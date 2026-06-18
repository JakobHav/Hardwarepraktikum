// Task 1: Environmental Light Categorization
// Reads the external Grove analog light sensor on A0.

#include "delay.h"
#include "wiring_analog.h"
#include <Adafruit_TinyUSB.h>
#define bright 50UL
#define dark 3500UL
#define depth 2

#define LIGHT_PIN A0

#define lowThresh 301
#define highThresh 70

unsigned long start;

// Categorizing the normalized value
const char *categorize(int normalized) {
  if (normalized <= map(highThresh, bright, dark, 0, 100)) {
    return "LOW";
  } else if (normalized > map(lowThresh, bright, dark, 0, 100)) {
    return "HIGH";
  } else {
    return "MEDIUM";
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial || millis() < 3000)
    ;

  analogReadResolution(12);

  start = millis();
}

void loop() {
  if (millis() - start >= 500) {
    start = millis();

    unsigned long raw = analogRead(LIGHT_PIN);

    raw = constrain(raw, bright, dark);

    unsigned long normalized = map(raw, bright, dark, 0, 100);

    const char *cat = categorize(normalized);

    Serial.print("R=");
    Serial.print(raw);
    Serial.print(", N=");
    Serial.print(normalized);
    Serial.print(", C=");
    Serial.println(cat);
  }
}