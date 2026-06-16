// Task 1: Environmental Light Categorization
// Complete the implementation according to the task description.

// Defining necessary constants (e.g., calibration range, thresholds,
// timing)

#include "delay.h"
#define bright 50UL
#define dark 3000UL
#define depth 2

#define SAADC 0x40007000
#define RESOLUTION_OFFSET 0x5F0

#define lowThresh 301
#define highThresh 70

unsigned long start;

// Categorizing the normalized value
const char *categorize(int normalized) {
  if (normalized <= map(highThresh, bright, dark, 0, 100)) {

    return "HIGH";
  } else if (normalized > map(lowThresh, bright, dark, 0, 100)) {

    return "LOW";
  } else {

    return "MEDIUM";
  }
}

void setup() {
  // Initializing Serial communication
  Serial.begin(115200);
  while (!Serial || millis() < 3000)
    ;

  // Configureing ADC resolution to 12-bit

  *(char *)(SAADC + RESOLUTION_OFFSET) = depth;
  start = millis();
}

void loop() {
  // Non-blockingly sampleing every 500 ms
  if (millis() - start >= 500) {
    start = millis();

    // Reading raw value from light sensor
    unsigned long value = analogRead(1);

    // Clamping the raw value to a calibrated range
    constrain(value, dark, bright);

    // Normalizing the value to a 0–100 scale
    unsigned long normalized = map(value, bright, dark, 0, 100);

    // Determining the category using the categorize() function
    const char *cat = categorize(normalized);

    // Printing raw value, normalized value, and category to Serial

    Serial.print("R=");
    Serial.print(value);
    Serial.print(", N=");
    Serial.print(normalized);
    Serial.print(", C=");
    Serial.println(cat);
    start = millis();
  }
}
