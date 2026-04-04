# E-Ink Weather display

Project around the waveshare e-ink display ([13353](https://www.waveshare.com/4.2inch-e-paper-module.htm)). A JSON API (e.g. ioBroker) provides the data that will be retrieved by the display. Sketch is intended to be used with a Wemos Lolin D1 mini ESP-8266.

## Connection diagram (Display: ESP)
- VCC: 5V
- GND: GND
- DIN (MOSI): D7 = GPIO 13
- CLK (SCK): D5 = GPIO 14
- CS: D8 = GPIO 15
- DC: D6 = GPIO 12
- RST: D2 = GPIO 4
- BUSY: D1 = GPIO 5

# Initialize
- copy secrets.example.h to secrets.h and fill in the details

## API
The JSON API/URL should reply with a 200er status code on a GET request with the following content:

```
{
    "prevent_update": false,
    "today": {
        "day": "04.04.2026 - Saturday",
        "front_temp": 11.7,
        "front_hum": 94,
        "back_temp": 12.21,
        "back_hum": 89.27,
        "rain_mm": 1,
        "rain_percent": 70,
        "pv_forecast": 20.123,
        "pv_current": "138W",
        "weather_warning": ""
    },
    "tomorrow": {
        "day": "Su (5.4.)",
        "temp_min": 5.78,
        "temp_max": 18.5,
        "rain_mm": 0,
        "rain_percent": 20,
        "pv_forecast": 25.324
    },
    "day2": {
        "day": "Mo (6.4.)",
        "temp_min": 3.28,
        "temp_max": 13.38,
        "rain_mm": 0,
        "rain_percent": 0
    },
    "timestamp": "04.04. 18:53"
}
```