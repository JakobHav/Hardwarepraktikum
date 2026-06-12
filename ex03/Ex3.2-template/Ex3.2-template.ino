// Task 2: Robust Temperature and Humidity Monitoring

#include <DHT.h>
#include <math.h>

#define a 17.62f
#define b 243.12f
// --- Configuration ---
// TODO: define sensor pin and type (DHT11)
//

// --- Objects ---
// TODO: create DHT sensor instance

// --- Timing ---
unsigned long lastSample = 0;
// TODO: define sampling interval (2 seconds)
#define sampleInterval 2000UL

// --- State Variables ---
// TODO: store last valid temperature and humidity
// TODO: maintain a failure counter

// --- Computation ---
float computeDewPoint(float tempC, float relHum) {
  // Calculating dew point via Magnus formula using natural logarithm
  float gamma = log(relHum / 100.0f) + (a * tempC) / (b + tempC);

  return (b * gamma) / (a - gamma);
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    ;
  // TODO: initialize sensor
}

void loop() {
  unsigned long start = millis();
  if (millis() - start >= 500) {

    // TODO: implement non-blocking sampling (2 s)

    // TODO: read temperature and humidity

    // TODO: handle invalid readings (NaN)
    // - update failure counter (reset on success)
    // - reuse last valid values if needed

    // TODO: compute dew point, print formatted output. If failure count exceeds
    // threshold print a warning.
  }
}
