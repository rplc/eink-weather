#pragma once

#include <Arduino.h>

// ============================================================================
// Datenstrukturen fuer die vom API gelieferten Wetter-/Hausdaten
// ============================================================================

struct Today
{
    String day;
    float front_temp;
    float front_hum;
    float back_temp;
    float back_hum;
    float rain_mm;
    int rain_percent;
    float pv_forecast;
    String pv_current;
    String weather_warning;
};

struct Forecast
{
    String day;
    float temp_min;
    float temp_max;
    float rain_mm;
    int rain_percent;
    float pv_forecast;
};

struct WeatherData
{
    bool preventUpdate = false;
    String timestamp;
    Today today;
    Forecast tomorrow;
    Forecast day2;
};
