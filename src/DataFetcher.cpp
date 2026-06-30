#include "DataFetcher.h"
#include "Config.h"

#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

namespace DataFetcher
{
    bool fetch(WeatherData &out)
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
            http.end();
            return false;
        }

        DynamicJsonDocument doc(2048);
        DeserializationError parseError = deserializeJson(doc, http.getString());
        http.end();

        if (parseError)
        {
            Serial.print("JSON parse error: ");
            Serial.println(parseError.c_str());
            return false;
        }

        out.preventUpdate = doc["prevent_update"];
        out.timestamp = doc["timestamp"].as<String>();

        out.today.day = doc["today"]["day"].as<String>();
        out.today.front_temp = doc["today"]["front_temp"];
        out.today.front_hum = doc["today"]["front_hum"];
        out.today.back_temp = doc["today"]["back_temp"];
        out.today.back_hum = doc["today"]["back_hum"];
        out.today.rain_mm = doc["today"]["rain_mm"];
        out.today.rain_percent = doc["today"]["rain_percent"];
        out.today.pv_forecast = doc["today"]["pv_forecast"];
        out.today.pv_current = doc["today"]["pv_current"].as<String>();
        out.today.weather_warning = doc["today"]["weather_warning"].as<String>();

        out.tomorrow.day = doc["tomorrow"]["day"].as<String>();
        out.tomorrow.temp_min = doc["tomorrow"]["temp_min"];
        out.tomorrow.temp_max = doc["tomorrow"]["temp_max"];
        out.tomorrow.rain_mm = doc["tomorrow"]["rain_mm"];
        out.tomorrow.rain_percent = doc["tomorrow"]["rain_percent"];
        out.tomorrow.pv_forecast = doc["tomorrow"]["pv_forecast"];

        out.day2.day = doc["day2"]["day"].as<String>();
        out.day2.temp_min = doc["day2"]["temp_min"];
        out.day2.temp_max = doc["day2"]["temp_max"];
        out.day2.rain_mm = doc["day2"]["rain_mm"];
        out.day2.rain_percent = doc["day2"]["rain_percent"];
        out.day2.pv_forecast = doc["day2"]["pv_forecast"];

        Serial.println("Data fetched and parsed.");
        return true;
    }
}
