#pragma once

#include <Arduino.h>

namespace WifiManager
{
    // Initialisiert den WLAN-Modus (einmalig im setup() aufzurufen).
    void begin();

    // Stellt sicher, dass eine WLAN-Verbindung besteht; verbindet bei Bedarf neu.
    void ensureConnected();
}
