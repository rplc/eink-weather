#pragma once

// ============================================================================
// Kapselt ArduinoOTA, damit Over-The-Air-Updates per
// `pio run -e d1_mini_ota -t upload` eingespielt werden koennen.
// ============================================================================

namespace OtaManager
{
    // Einmalig in setup() aufrufen (nach WifiManager::begin()).
    void begin();

    // In jedem loop()-Durchlauf aufrufen, damit eingehende OTA-Updates
    // verarbeitet werden koennen.
    void handle();
}
