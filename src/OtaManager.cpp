#include "OtaManager.h"
#include "Config.h"

#include <Arduino.h>
#include <ArduinoOTA.h>

namespace OtaManager
{
    void begin()
    {
        ArduinoOTA.setHostname(OTA_HOSTNAME);
        ArduinoOTA.setPassword(OTA_PASSWORD);

        ArduinoOTA.onStart([]() {
            String type = (ArduinoOTA.getCommand() == U_FLASH) ? "Sketch" : "Filesystem";
            Serial.println("OTA Update gestartet: " + type);
        });

        ArduinoOTA.onEnd([]() {
            Serial.println("\nOTA Update abgeschlossen.");
        });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("OTA Fortschritt: %u%%\r", (progress / (total / 100)));
        });

        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("OTA Fehler [%u]: ", error);
            switch (error)
            {
                case OTA_AUTH_ERROR:    Serial.println("Auth Failed");    break;
                case OTA_BEGIN_ERROR:   Serial.println("Begin Failed");   break;
                case OTA_CONNECT_ERROR: Serial.println("Connect Failed"); break;
                case OTA_RECEIVE_ERROR: Serial.println("Receive Failed"); break;
                case OTA_END_ERROR:     Serial.println("End Failed");     break;
            }
        });

        ArduinoOTA.begin();
        Serial.println("OTA bereit unter Hostname: " OTA_HOSTNAME);
    }

    void handle()
    {
        ArduinoOTA.handle();
    }
}
