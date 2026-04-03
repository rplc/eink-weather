#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>

GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display(
  GxEPD2_420_M01(15, 12, 4, 5)
);

void setup() {
  Serial.begin(115200);
  delay(100);

  display.init(115200);
  display.setRotation(0);

  display.setTextColor(GxEPD_BLACK);

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    display.setCursor(20, 40);
    display.println("ESP32 D1 Mini");

    display.setCursor(20, 80);
    display.println("Waveshare 4.2\"");

    display.setCursor(20, 120);
    display.println("Hello eInk!");

  } while (display.nextPage());

  display.hibernate();
}

void loop() {}