#include <Arduino.h>

#include "Config.h"
#include "WeatherData.h"
#include "WifiManager.h"
#include "OtaManager.h"
#include "DataFetcher.h"
#include "DisplayRenderer.h"

WeatherData weatherData;

void setup()
{
    Serial.begin(115200);

    DisplayRenderer::begin();

    WifiManager::begin();
    OtaManager::begin();
}

void loop()
{
    WifiManager::ensureConnected();
    OtaManager::handle();

    if (DataFetcher::fetch(weatherData))
    {
        DisplayRenderer::draw(weatherData);
    }
    else
    {
        Serial.println("Fehler beim Laden der Daten");
    }

    // Waehrend der Wartezeit regelmaessig OTA-Anfragen verarbeiten,
    // statt das Geraet mit einem einzigen langen delay() unerreichbar zu machen.
    unsigned long waitStart = millis();
    while (millis() - waitStart < UPDATE_INTERVAL_MS)
    {
        OtaManager::handle();
        delay(50);
    }
}
