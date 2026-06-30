#include "DisplayRenderer.h"
#include "Config.h"

#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>

// ============================================================================
// Layout
// ----------------------------------------------------------------------------
// Damit Partial Updates funktionieren, hat jeder Block eine FESTE Position
// und Groesse (im Original wurde die Y-Position dynamisch verschoben, z.B.
// wenn die Wetterwarnung fehlte -- das macht Partial-Diffing unmoeglich,
// da sich dann ploetzlich Inhalte ausserhalb des erwarteten Blocks befinden).
//
// Spalten:
//   Links  (Heute):    x  10 .. 195   (Trennlinie bei x=200, bleibt unberuehrt)
//   Rechts (Morgen/+2): x 205 .. 400
//
// Jeder Block bekommt ein Rechteck, das beim Partial-Update als
// "Fenster" an display.setPartialWindow() uebergeben wird.
// ============================================================================

namespace
{
    GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display(
        GxEPD2_420_M01(PIN_CS, PIN_DC, PIN_RST, PIN_BUSY));

    const char DEGREE_SYMBOL = (char)247;

    struct Region
    {
        int x, y, w, h;
    };

    // Kopfzeile (Wochentag "heute" + horizontale Trennlinie bei y=35)
    const Region REGION_HEADER     = {0,   0,  400,  40};

    // Linke Spalte ("Heute")
    const Region REGION_FRONT      = {10,  50, 190,  50};
    const Region REGION_BACK       = {10, 100, 190,  50};
    const Region REGION_RAIN       = {10, 150, 190,  50};
    const Region REGION_PV         = {10, 200, 190,  50};
    const Region REGION_WARNING    = {10, 250, 190,  50};

    // Rechte Spalte ("Morgen" / "Uebermorgen"), inkl. horizontaler
    // Trennlinie bei y=170 zwischen den beiden Bloecken (wird in beiden
    // angrenzenden Bloecken mitgezeichnet, damit sie nie verschwindet).
    const Region REGION_TOMORROW   = {205, 45, 195, 130};
    const Region REGION_DAY2       = {205, 175, 195, 95};

    const Region REGION_TIMESTAMP  = {300, 280, 100, 20};

    // Letzter tatsaechlich auf dem Display angezeigter Stand, fuer das Diffing.
    WeatherData lastDrawn;
    bool hasDrawn = false;
    int updatesSinceFullRefresh = 0;

    // ------------------------------------------------------------------
    // Hilfsfunktionen
    // ------------------------------------------------------------------

    void drawCenteredText(const String &text, int y)
    {
        int16_t x1, y1;
        uint16_t w, h;

        display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
        int x = (DISPLAY_WIDTH - w) / 2;

        display.setCursor(x, y);
        display.print(text);
    }

    // Setzt das Partial-Window; rundet x/Breite auf volle Bytes (8px), wie
    // von den meisten SSD16xx-Controllern gefordert.
    void setWindow(const Region &r)
    {
        int x0 = (r.x / 8) * 8;
        int x1 = ((r.x + r.w + 7) / 8) * 8;
        if (x1 > DISPLAY_WIDTH) x1 = DISPLAY_WIDTH;
        display.setPartialWindow(x0, r.y, x1 - x0, r.h);
    }

    // ------------------------------------------------------------------
    // Block-Zeichenfunktionen (zeichnen IMMER mit absoluten Display-
    // Koordinaten, unabhaengig davon ob Full- oder Partial-Window aktiv ist)
    // ------------------------------------------------------------------

    void drawHeaderBlock(const WeatherData &data)
    {
        display.setTextSize(2);
        drawCenteredText(data.today.day, 10);
        display.drawLine(0, 35, DISPLAY_WIDTH, 35, GxEPD_BLACK);
    }

    void drawFrontBlock(const WeatherData &data)
    {
        const int x = 20;
        display.setCursor(x, 55);
        display.setTextSize(1);
        display.print("Front: ");

        display.setCursor(x, 70);
        display.setTextSize(2);
        display.printf("%.1f", data.today.front_temp);
        display.print(DEGREE_SYMBOL);
        display.printf("C %.0f%%", data.today.front_hum);
    }

    void drawBackBlock(const WeatherData &data)
    {
        const int x = 20;
        display.setCursor(x, 105);
        display.setTextSize(1);
        display.print("Back: ");

        display.setCursor(x, 120);
        display.setTextSize(2);
        display.printf("%.1f", data.today.back_temp);
        display.print(DEGREE_SYMBOL);
        display.printf("C %.0f%%", data.today.back_hum);
    }

    void drawRainBlock(const WeatherData &data)
    {
        const int x = 20;
        display.setCursor(x, 155);
        display.setTextSize(1);
        display.print("Rain: ");

        display.setCursor(x, 170);
        display.setTextSize(2);
        display.printf("%.1fmm %d%%", data.today.rain_mm, data.today.rain_percent);
    }

    void drawPvBlock(const WeatherData &data)
    {
        const int x = 20;
        display.setCursor(x, 205);
        display.setTextSize(1);
        display.printf("PV Forecast %.2fkWh", data.today.pv_forecast);

        display.setCursor(x, 220);
        display.setTextSize(2);
        display.print(data.today.pv_current);
    }

    void drawWarningBlock(const WeatherData &data)
    {
        const int x = 20;
        if (data.today.weather_warning.length() > 0)
        {
            display.setCursor(x, 255);
            display.setTextSize(1);
            display.print("Caution: ");

            display.setCursor(x, 270);
            display.setTextSize(2);
            display.print(data.today.weather_warning);
        }
        // Wenn keine Warnung vorliegt, bleibt der Block einfach leer/weiss
        // (Bereich wurde vorher per fillScreen(WHITE) im Fenster geleert).
    }

    void drawForecastBlock(const Forecast &forecast, int xCol, int yStart)
    {
        int yPos = yStart;

        display.setTextSize(2);
        display.setCursor(xCol, yPos);
        display.print(forecast.day);
        yPos += 30;

        display.setTextSize(1);
        display.setCursor(xCol, yPos);
        display.printf("%.0f", forecast.temp_min);
        display.print(DEGREE_SYMBOL);
        display.print("C - ");
        display.printf("%.0f", forecast.temp_max);
        display.print(DEGREE_SYMBOL);
        display.print("C");
        yPos += 20;

        display.setCursor(xCol, yPos);
        display.printf("%.1fmm %d%%", forecast.rain_mm, forecast.rain_percent);
        yPos += 20;

        display.setCursor(xCol, yPos);
        display.printf("%.2fkWh", forecast.pv_forecast);
    }

    void drawTomorrowBlock(const WeatherData &data)
    {
        drawForecastBlock(data.tomorrow, 230, 70);
        // Horizontale Trennlinie zum "Uebermorgen"-Block (gehoert geometrisch
        // dazwischen, wird hier UND im Day2-Block gezeichnet, damit sie bei
        // Partial-Updates beider Bloecke nie verschwindet).
        display.drawLine(205, 170, DISPLAY_WIDTH, 170, GxEPD_BLACK);
    }

    void drawDay2Block(const WeatherData &data)
    {
        display.drawLine(205, 170, DISPLAY_WIDTH, 170, GxEPD_BLACK);

        int yPos = 200;
        display.setTextSize(2);
        display.setCursor(230, yPos);
        display.print(data.day2.day);
        yPos += 30;

        display.setTextSize(1);
        display.setCursor(230, yPos);
        display.printf("%.0f", data.day2.temp_min);
        display.print(DEGREE_SYMBOL);
        display.print("C - ");
        display.printf("%.0f", data.day2.temp_max);
        display.print(DEGREE_SYMBOL);
        display.print("C");
        yPos += 20;

        display.setCursor(230, yPos);
        display.printf("%.1fmm %d%%", data.day2.rain_mm, data.day2.rain_percent);
    }

    void drawTimestampBlock(const WeatherData &data)
    {
        display.setCursor(325, 293);
        display.setTextSize(1);
        display.print(data.timestamp);
    }

    void drawStaticBorders()
    {
        display.drawLine(0, 35, DISPLAY_WIDTH, 35, GxEPD_BLACK);
        display.drawLine(200, 40, 200, 300, GxEPD_BLACK);
        display.drawLine(205, 170, DISPLAY_WIDTH, 170, GxEPD_BLACK);
    }

    // ------------------------------------------------------------------
    // Vergleichsfunktionen: hat sich der jeweilige Block inhaltlich
    // gegenueber dem zuletzt gezeichneten Stand veraendert?
    // ------------------------------------------------------------------

    bool headerChanged(const WeatherData &a, const WeatherData &b)
    {
        return a.today.day != b.today.day;
    }

    bool frontChanged(const WeatherData &a, const WeatherData &b)
    {
        return a.today.front_temp != b.today.front_temp ||
               a.today.front_hum != b.today.front_hum;
    }

    bool backChanged(const WeatherData &a, const WeatherData &b)
    {
        return a.today.back_temp != b.today.back_temp ||
               a.today.back_hum != b.today.back_hum;
    }

    bool rainChanged(const WeatherData &a, const WeatherData &b)
    {
        return a.today.rain_mm != b.today.rain_mm ||
               a.today.rain_percent != b.today.rain_percent;
    }

    bool pvChanged(const WeatherData &a, const WeatherData &b)
    {
        return a.today.pv_forecast != b.today.pv_forecast ||
               a.today.pv_current != b.today.pv_current;
    }

    bool warningChanged(const WeatherData &a, const WeatherData &b)
    {
        return a.today.weather_warning != b.today.weather_warning;
    }

    bool forecastChanged(const Forecast &a, const Forecast &b)
    {
        return a.day != b.day ||
               a.temp_min != b.temp_min ||
               a.temp_max != b.temp_max ||
               a.rain_mm != b.rain_mm ||
               a.rain_percent != b.rain_percent ||
               a.pv_forecast != b.pv_forecast;
    }

    bool timestampChanged(const WeatherData &a, const WeatherData &b)
    {
        return a.timestamp != b.timestamp;
    }

    // ------------------------------------------------------------------
    // Zeichnet einen einzelnen Block als Partial-Update (eigenes Fenster).
    // ------------------------------------------------------------------
    template <typename DrawFn>
    void drawPartial(const Region &region, DrawFn drawFn, const WeatherData &data)
    {
        setWindow(region);
        display.firstPage();
        do
        {
            display.setTextColor(GxEPD_BLACK);
            display.fillScreen(GxEPD_WHITE);
            drawFn(data);
        } while (display.nextPage());
    }

    void fullRedraw(const WeatherData &data)
    {
        Serial.println("Full refresh.");
        display.setFullWindow();
        display.firstPage();
        do
        {
            display.setTextColor(GxEPD_BLACK);
            display.fillScreen(GxEPD_WHITE);

            drawHeaderBlock(data);
            drawFrontBlock(data);
            drawBackBlock(data);
            drawRainBlock(data);
            drawPvBlock(data);
            drawWarningBlock(data);
            drawStaticBorders();
            drawTomorrowBlock(data);
            drawDay2Block(data);
            drawTimestampBlock(data);

        } while (display.nextPage());
    }

    void partialRedraw(const WeatherData &data)
    {
        bool anyChange = false;

        if (headerChanged(data, lastDrawn))
        {
            drawPartial(REGION_HEADER, drawHeaderBlock, data);
            anyChange = true;
        }
        if (frontChanged(data, lastDrawn))
        {
            drawPartial(REGION_FRONT, drawFrontBlock, data);
            anyChange = true;
        }
        if (backChanged(data, lastDrawn))
        {
            drawPartial(REGION_BACK, drawBackBlock, data);
            anyChange = true;
        }
        if (rainChanged(data, lastDrawn))
        {
            drawPartial(REGION_RAIN, drawRainBlock, data);
            anyChange = true;
        }
        if (pvChanged(data, lastDrawn))
        {
            drawPartial(REGION_PV, drawPvBlock, data);
            anyChange = true;
        }
        if (warningChanged(data, lastDrawn))
        {
            drawPartial(REGION_WARNING, drawWarningBlock, data);
            anyChange = true;
        }
        if (forecastChanged(data.tomorrow, lastDrawn.tomorrow))
        {
            drawPartial(REGION_TOMORROW, drawTomorrowBlock, data);
            anyChange = true;
        }
        if (forecastChanged(data.day2, lastDrawn.day2))
        {
            drawPartial(REGION_DAY2, drawDay2Block, data);
            anyChange = true;
        }
        if (timestampChanged(data, lastDrawn))
        {
            drawPartial(REGION_TIMESTAMP, drawTimestampBlock, data);
            anyChange = true;
        }

        if (!anyChange)
        {
            Serial.println("Keine Aenderungen, Display bleibt unveraendert.");
        }
    }
}

namespace DisplayRenderer
{
    void begin()
    {
        display.init();
        display.setRotation(0);
        display.setFont(NULL);
    }

    void draw(const WeatherData &data)
    {
        if (data.preventUpdate)
        {
            Serial.println("Preventing update via data, no UI to draw.");
            return;
        }

        bool partialSupported = display.epd2.hasPartialUpdate;
        bool forceFull = !hasDrawn ||
                          !partialSupported ||
                          updatesSinceFullRefresh >= FULL_REFRESH_EVERY_N_UPDATES;

        if (forceFull)
        {
            fullRedraw(data);
            updatesSinceFullRefresh = 0;
        }
        else
        {
            Serial.printf("Partial refresh (%d/%d seit letztem Full-Refresh).\n",
                          updatesSinceFullRefresh + 1, FULL_REFRESH_EVERY_N_UPDATES);
            partialRedraw(data);
            updatesSinceFullRefresh++;
        }

        lastDrawn = data;
        hasDrawn = true;
    }
}
