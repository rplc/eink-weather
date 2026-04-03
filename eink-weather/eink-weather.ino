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
struct Today
{
    String day;
    float front_temp;
    float front_hum;
    float back_temp;
    float back_hum;
    float rain_mm;
    int rain_percent;
    float pv;
};

struct Forecast
{
    String day;
    float temp_min;
    float temp_max;
    float rain_mm;
    int rain_percent;
    float pv;
};

Today today;
Forecast tomorrow, day2;
String timestamp;

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

    timestamp = doc["timestamp"].as<String>();

    today.day = doc["today"]["day"].as<String>();
    today.front_temp = doc["today"]["front_temp"];
    today.front_hum = doc["today"]["front_hum"];
    today.back_temp = doc["today"]["back_temp"];
    today.back_hum = doc["today"]["back_hum"];
    today.rain_mm = doc["today"]["rain_mm"];
    today.rain_percent = doc["today"]["rain_percent"];
    today.pv = doc["today"]["pv"];

    tomorrow.day = doc["tomorrow"]["day"].as<String>();
    tomorrow.temp_min = doc["tomorrow"]["temp_min"];
    tomorrow.temp_max = doc["tomorrow"]["temp_max"];
    tomorrow.rain_mm = doc["tomorrow"]["rain_mm"];
    tomorrow.rain_percent = doc["tomorrow"]["rain_percent"];
    tomorrow.pv = doc["tomorrow"]["pv"];

    day2.day = doc["day2"]["day"].as<String>();
    day2.temp_min = doc["day2"]["temp_min"];
    day2.temp_max = doc["day2"]["temp_max"];
    day2.rain_mm = doc["day2"]["rain_mm"];
    day2.rain_percent = doc["day2"]["rain_percent"];
    day2.pv = doc["day2"]["pv"];

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
        display.setTextColor(GxEPD_BLACK);
        display.fillScreen(GxEPD_WHITE);

        // HEADER (größere Schrift)
        display.setTextSize(2);
        drawCenteredText(today.day, 10);

        // Trennlinie
        display.drawLine(0, 35, 400, 35, GxEPD_BLACK);

        // SPALTEN
        int xToday = 30;
        int xTomorrow = 230;
        int yPos = 70;

        // HEUTE (größere Schrift)
        display.setCursor(xToday, yPos);
        display.setTextSize(1);
        display.print("Front: ");
        yPos += 15;

        display.setCursor(xToday, yPos);
        display.setTextSize(2);

        display.printf("%.1f", today.front_temp);
        display.print((char)247);
        display.printf("C %.0f%%", today.front_hum);
        yPos += 30;

        display.setCursor(xToday, yPos);
        display.setTextSize(1);
        display.print("Back: ");
        yPos += 15;

        display.setCursor(xToday, yPos);
        display.setTextSize(2);

        display.printf("%.1f", today.back_temp);
        display.print((char)247);
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
        display.print("PV Forecast: ");
        yPos += 15;

        display.setTextSize(2);
        display.setCursor(xToday, yPos);
        display.printf("%.2fkWh", today.pv);
        yPos += 30;

        // TRENNLINIE VERTIKAL
        display.drawLine(200, 40, 200, 300, GxEPD_BLACK);

        // MORGEN
        yPos = 70;
        display.setTextSize(2);
        display.setCursor(xTomorrow, yPos);
        display.print(tomorrow.day);
        yPos += 30;

        display.setTextSize(1);
        display.setCursor(xTomorrow, yPos);
        display.printf("%.0f", tomorrow.temp_min);
        display.print((char)247);
        display.print("C - ");
        display.printf("%.0f", tomorrow.temp_max);
        display.print((char)247);
        display.print("C");
        yPos += 20;

        display.setCursor(xTomorrow, yPos);
        display.printf("%.1fmm %d%%", tomorrow.rain_mm, tomorrow.rain_percent);
        yPos += 20;

        display.setCursor(xTomorrow, yPos);
        display.printf("%.2fkWh", tomorrow.pv);

        // TRENNLINIE HORIZONTAL
        display.drawLine(205, 170, 400, 170, GxEPD_BLACK);

        // DAY2
        yPos = 200;
        display.setTextSize(2);
        display.setCursor(xTomorrow, yPos);
        display.print(day2.day);
        yPos += 30;

        display.setTextSize(1);
        display.setCursor(xTomorrow, yPos);
        display.printf("%.0f", day2.temp_min);
        display.print((char)247);
        display.print("C - ");
        display.printf("%.0f", day2.temp_max);
        display.print((char)247);
        display.print("C");
        yPos += 20;

        display.setCursor(xTomorrow, yPos);
        display.printf("%.1fmm %d%%", day2.rain_mm, day2.rain_percent);

        display.setCursor(325, 293);
        display.setTextSize(1);
        display.print(timestamp);

    } while (display.nextPage());

    display.hibernate();
}

void setup()
{
    Serial.begin(115200);

    display.init();
    display.setRotation(0);
    display.setFont(NULL);

    connectWiFi();

    if (fetchData())
    {
        drawUI();
    }

    ESP.deepSleep(5 * 60 * 1000000);
}

void loop() {}