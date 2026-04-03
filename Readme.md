# E-Ink Weather display

Project around the waveshare e-ink display ([13353](https://www.waveshare.com/4.2inch-e-paper-module.htm)). ioBroker provides the data that will be retrieved by the display. Sketch is intended to be used with a Wemos Lolin D1 mini ESP-8266.

## Connection diagram
- VCC: 5V
- GND: GND
- DIN (MOSI): D7 = 13
- CLK (SCK): D5 = 14
- CS: D8 = 15
- DC: D6 = 12
- RST: D2 = 4
- BUSY: D1 = 5

## Deep sleep
After a display refresh ESP enters in deep sleep for 5 minutes. For the deep sleep to work you need to *connect D0 PIN to the RST PIN*.

# Initialize
- copy secrets.example.h to secrets.h and fill in the details