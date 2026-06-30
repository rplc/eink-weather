#include "DisplayRenderer.h"
#include "Config.h"

#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>

namespace
{
    GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display(
        GxEPD2_420_M01(PIN_CS, PIN_DC, PIN_RST, PIN_BUSY));

    const char DEGREE_SYMBOL = (char)247;

    void drawCenteredText(const String &text, int y)
    {
        int16_t x1, y1;
        uint16_t w, h;

        display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
        int x = (DISPLAY_WIDTH - w) / 2;

        display.setCursor(x, y);
        display.print(text);
    }

    void drawHeader(const WeatherData &data)
    {
        display.setTextSize(2);
        drawCenteredText(data.today.day, 10);
        display.drawLine(0, 35, DISPLAY_WIDTH, 35, GxEPD_BLACK);
    }

    void drawTodayColumn(const Today &today, int xToday, int &yPos)
    {
        display.setCursor(xToday, yPos);
        display.setTextSize(1);
        display.print("Front: ");
        yPos += 15;

        display.setCursor(xToday, yPos);
        display.setTextSize(2);
        display.printf("%.1f", today.front_temp);
        display.print(DEGREE_SYMBOL);
        display.printf("C %.0f%%", today.front_hum);
        yPos += 30;

        display.setCursor(xToday, yPos);
        display.setTextSize(1);
        display.print("Back: ");
        yPos += 15;

        display.setCursor(xToday, yPos);
        display.setTextSize(2);
        display.printf("%.1f", today.back_temp);
        display.print(DEGREE_SYMBOL);
        display.printf("C %.0f%%", today.back_hum);
        yPos += 30;

        display.setCursor(xToday, yPos);
        display.setTextSize(1);
        display.print("Rain: ");
        yPos += 15;

        display.setTextSize(2);
        display.setCursor(xToday, yPos);
        display.printf("%.1fmm %d%%", today.rain_mm, today.rain_percent);
        yPos += 30;

        display.setCursor(xToday, yPos);
        display.setTextSize(1);
        display.printf("PV Forecast %.2fkWh", today.pv_forecast);
        yPos += 15;

        display.setTextSize(2);
        display.setCursor(xToday, yPos);
        display.print(today.pv_current);
        yPos += 30;

        if (today.weather_warning.length() > 0)
        {
            display.setCursor(xToday, yPos);
            display.setTextSize(1);
            display.print("Caution: ");
            yPos += 15;

            display.setTextSize(2);
            display.setCursor(xToday, yPos);
            display.print(today.weather_warning);
            yPos += 30;
        }
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

        Serial.println("Drawing ui.");
        display.setFullWindow();

        const int xToday = 30;
        const int xTomorrow = 230;

        display.firstPage();
        do
        {
            display.setTextColor(GxEPD_BLACK);
            display.fillScreen(GxEPD_WHITE);

            drawHeader(data);

            int yPos = 70;
            drawTodayColumn(data.today, xToday, yPos);

            // Vertikale Trennlinie zwischen den Spalten
            display.drawLine(200, 40, 200, 300, GxEPD_BLACK);

            // Morgen
            drawForecastBlock(data.tomorrow, xTomorrow, 70);

            // Horizontale Trennlinie zwischen Morgen und Uebermorgen
            display.drawLine(205, 170, DISPLAY_WIDTH, 170, GxEPD_BLACK);

            // Uebermorgen (day2): nur Tag + Temperatur + Regen, kein PV-Wert
            // (wie im Original-Layout)
            int yDay2 = 200;
            display.setTextSize(2);
            display.setCursor(xTomorrow, yDay2);
            display.print(data.day2.day);
            yDay2 += 30;

            display.setTextSize(1);
            display.setCursor(xTomorrow, yDay2);
            display.printf("%.0f", data.day2.temp_min);
            display.print(DEGREE_SYMBOL);
            display.print("C - ");
            display.printf("%.0f", data.day2.temp_max);
            display.print(DEGREE_SYMBOL);
            display.print("C");
            yDay2 += 20;

            display.setCursor(xTomorrow, yDay2);
            display.printf("%.1fmm %d%%", data.day2.rain_mm, data.day2.rain_percent);

            display.setCursor(325, 293);
            display.setTextSize(1);
            display.print(data.timestamp);

        } while (display.nextPage());
    }
}
