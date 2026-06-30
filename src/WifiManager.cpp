#include "WifiManager.h"
#include "Config.h"

#include <ESP8266WiFi.h>

namespace WifiManager
{
    void begin()
    {
        WiFi.mode(WIFI_STA);
        ensureConnected();
    }

    void ensureConnected()
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            return;
        }

        Serial.println("Reconnect WLAN...");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        int timeout = 0;
        while (WiFi.status() != WL_CONNECTED && timeout < 20)
        {
            delay(500);
            Serial.print(".");
            timeout++;
        }

        Serial.println(WiFi.status() == WL_CONNECTED ? " OK" : " FAIL");

        Serial.print("IP-Adresse: ");
        Serial.println(WiFi.localIP());
    }
}
