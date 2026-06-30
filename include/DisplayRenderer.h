#pragma once

#include "WeatherData.h"

namespace DisplayRenderer
{
    // Initialisiert das E-Ink Display. Einmalig in setup() aufrufen.
    void begin();

    // Zeichnet die komplette UI fuer die uebergebenen Daten.
    // Macht nichts, wenn data.preventUpdate gesetzt ist.
    void draw(const WeatherData &data);
}
