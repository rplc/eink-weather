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

## Deep sleep
After a display refresh ESP enters in deep sleep for 5 minutes. For the deep sleep to work you need to **connect D0 PIN to the RST PIN**.

**CAUTION**: After connecting D0 - RST PINs you cannot flash a new sketch/firmware. Before that these PINs must be disconnected!!!.

# Initialize
- copy secrets.example.h to secrets.h and fill in the details

## API
The JSON API/URL should reply with a 200er status code on a GET request with the following content:

```
{
    "today": {
        "day": "03.04.2026 - Friday",
        "front_temp": 10.9,
        "front_hum": 52,
        "back_temp": 11.34,
        "back_hum": 57.17,
        "rain_mm": 0,
        "rain_percent": 20,
        "pv": 20.299
    },
    "tomorrow": {
        "day": "Sa (4.4.)",
        "temp_min": 7.08,
        "temp_max": 15.1,
        "rain_mm": 0.6,
        "rain_percent": 60,
        "pv": 20.368
    },
    "day2":{
        "day": "Su (5.4.)",
        "temp_min": 4.76,
        "temp_max": 18.91,
        "rain_mm": 0,
        "rain_percent": 20
    },
    "timestamp": "03.04. 19:48"
}
```