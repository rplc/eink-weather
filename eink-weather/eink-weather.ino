#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "secrets.h"

// PINS
#define CS 15
#define DC 12
#define RST 4
#define BUSY 5

GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display(
    GxEPD2_420_M01(CS, DC, RST, BUSY));

// DATEN
struct DayData
{
    String day;
    float temp_min;
    float temp_max;
    float rain_mm;
    int rain_percent;
    int pv;
};

DayData today, tomorrow;
String currentDate;

int humidity_in = 0;
int humidity_out = 0;

void connectWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 30)
    {
        delay(500);
        timeout++;
    }

    Serial.println("Wifi connected.");
}

bool fetchData()
{
    Serial.println("Fetching data...");
    WiFiClient client;
    HTTPClient http;

    http.begin(client, API_URL);
    int code = http.GET();

    if (code != 200)
    {
        Serial.print("Fetching failed with status code:");
        Serial.println(code);
        return false;
    }

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, http.getString());

    currentDate = doc["date"].as<String>();

    today.temp_min = doc["today"]["min"];
    today.temp_max = doc["today"]["max"];
    today.rain_mm = doc["today"]["rain_mm"];
    today.rain_percent = doc["today"]["rain_percent"];
    today.pv = doc["today"]["pv"];

    humidity_in = doc["today"]["humidity_in"];
    humidity_out = doc["today"]["humidity_out"];

    tomorrow.day = doc["tomorrow"]["day"].as<String>();
    tomorrow.temp_min = doc["tomorrow"]["min"];
    tomorrow.temp_max = doc["tomorrow"]["max"];
    tomorrow.rain_mm = doc["tomorrow"]["rain_mm"];
    tomorrow.rain_percent = doc["tomorrow"]["rain_percent"];
    tomorrow.pv = doc["tomorrow"]["pv"];

    Serial.println("Data fetched and parsed.");
    return true;
}

void drawCenteredText(String text, int y)
{
    int16_t x1, y1;
    uint16_t w, h;

    display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);

    int x = (400 - w) / 2; // 400 = Displaybreite

    display.setCursor(x, y);
    display.print(text);
}

void drawUI()
{
    Serial.println("Drawing ui.");
    display.setFullWindow();

    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);

        // HEADER (größere Schrift)
        display.setTextSize(2);
        drawCenteredText(currentDate, 10);

        // Trennlinie
        display.drawLine(0, 35, 400, 35, GxEPD_BLACK);

        // SPALTEN
        int xToday = 10;
        int xTomorrow = 240;

        // HEUTE (größere Schrift)
        display.setTextSize(2);

        display.setCursor(xToday, 70);

        display.printf("%.0f", today.temp_max);
        display.print((char)247);
        display.print("C");

        display.setTextSize(1);
        display.setCursor(xToday, 95);
        display.printf("Min: %.0f", today.temp_min);
        display.print((char)247);
        display.print("C");

        // Regen
        display.setCursor(xToday, 120);
        display.printf("Regen: %.1fmm", today.rain_mm);

        display.setCursor(xToday, 140);
        display.printf("%d %%", today.rain_percent);

        // Luftfeuchtigkeit
        display.setCursor(xToday, 170);
        display.printf("Innen: %d%%", humidity_in);

        display.setCursor(xToday, 190);
        display.printf("Aussen: %d%%", humidity_out);

        // PV
        display.setTextSize(2);
        display.setCursor(xToday, 230);
        display.printf("%d kWh", today.pv);

        // TRENNLINIE VERTIKAL
        display.drawLine(200, 40, 200, 300, GxEPD_BLACK);

        // MORGEN
        display.setTextSize(2);
        display.setCursor(xTomorrow, 70);
        display.print(tomorrow.day);

        display.setTextSize(1);
        display.setCursor(xTomorrow, 100);
        display.printf("%.0f", tomorrow.temp_min);
        display.print((char)247);
        display.print("C - ");
        display.printf("%.0f", tomorrow.temp_max);
        display.print((char)247);
        display.print("C");

        display.setCursor(xTomorrow, 130);
        display.printf("%.1fmm", tomorrow.rain_mm);

        display.setCursor(xTomorrow, 150);
        display.printf("%d %%", tomorrow.rain_percent);

        display.setCursor(xTomorrow, 180);
        display.printf("%d kWh", tomorrow.pv);

    } while (display.nextPage());

    display.hibernate();
}

void setup()
{
    Serial.begin(115200);

    display.init();
    display.setRotation(0);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(NULL);

    connectWiFi();

    if (fetchData())
    {
        drawUI();
    }

    ESP.deepSleep(5 * 60 * 1000000);
}

void loop() {}