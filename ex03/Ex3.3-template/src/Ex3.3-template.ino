// Task 3: Air Quality Monitoring with SGP30

#include <Adafruit_SGP30.h>
#include <cstdint>

// --- Objects ---
Adafruit_SGP30 sgp;

// --- Timing ---
// define sampling interval (1 second)
#define SAMPLING_INTERVAL 1000UL

// --- Warm-up ---
// define warm-up duration (15 seconds)
#define WARM_UP_DURATION 15000UL
// maintain a warm-up state indicator
bool isWarmUp = true;

unsigned int start_time = 0;

const char *printMeasurement(uint16_t eCO2, uint16_t TVOC) {
  // if in warmup phase, print UNSTABLE/WARMUP; otherwise, print the actual
  // measurements

  static char unstable[18] = "[UNSTABLE/WARMUP]";
  static char ready[8] = "[READY]";

  static char buffer[100];

  snprintf(buffer, sizeof(buffer), "%s eCO2: %u ppm, TVOC: %u ppb", isWarmUp ? unstable : ready, eCO2, TVOC);

  return buffer;
}

bool measure() {
  if (sgp.IAQmeasure()) {
    const char *mes = printMeasurement(sgp.eCO2, sgp.TVOC);

    Serial.println(mes);
    return true;
  } else {
    Serial.println("Measurement failed");
    return false;
  }
}

unsigned long last_measurement = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    ;

  //  initialize the sensor and handle initialization failure
  sgp.begin();

  //  start air quality measurements
  sgp.IAQinit();

  //  record system start time
  start_time = millis();

  last_measurement = start_time;
}

void loop() {
  unsigned long now = millis();

  //  implement periodic sampling at 1 Hz using millis()

  if (now - last_measurement > SAMPLING_INTERVAL) {
    //  update warm-up state based on elapsed time

    if (now - start_time > WARM_UP_DURATION) {
      isWarmUp = false;
    }

    //  output measurement results
    //       - indicate warm-up vs ready state
    //       - ensure correct formatting for integer values

    measure();

    last_measurement = now; // reset the timer for the next measurement
  }
}
