#pragma once

#include "WeatherData.h"

namespace DataFetcher
{
    // Ruft die Daten vom konfigurierten API_URL ab und befuellt `out`.
    // Gibt true bei Erfolg zurueck, false bei HTTP- oder Parse-Fehlern.
    bool fetch(WeatherData &out);
}
