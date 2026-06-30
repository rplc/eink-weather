#pragma once

// --- E-Ink Display Pins (Wemos D1 Mini) ---
#define PIN_CS   15
#define PIN_DC   12
#define PIN_RST  4
#define PIN_BUSY 5

// --- Display-Geometrie ---
#define DISPLAY_WIDTH  400
#define DISPLAY_HEIGHT 300

// --- Update-Intervall (Millisekunden) ---
#define UPDATE_INTERVAL_MS (5UL * 60UL * 1000UL)

// --- Partial Updates ---
// Nach wie vielen Partial-Update-Zyklen erzwingen wir einen kompletten
// Full-Refresh; 12 Zyklen * 5 Minuten Update-Intervall = 1 Stunde.
#define FULL_REFRESH_EVERY_N_UPDATES 12