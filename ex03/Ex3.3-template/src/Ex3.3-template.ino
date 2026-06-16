// Task 3: Air Quality Monitoring with SGP30


#include <Adafruit_SGP30.h>

// --- Objects ---
Adafruit_SGP30 sgp;

// --- Timing ---
// TODO: define sampling interval (1 second)

// --- Warm-up ---
// TODO: define warm-up duration (15 seconds)
// TODO: maintain a warm-up state indicator

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    // TODO: initialize the sensor and handle initialization failure

    // TODO: start air quality measurements

    // TODO: record system start time
}

void loop() {
    unsigned long now = millis();

    // TODO: implement periodic sampling at 1 Hz using millis()

    // TODO: perform a measurement and handle failure cases

    // TODO: update warm-up state based on elapsed time

    // TODO: output measurement results
    //       - indicate warm-up vs ready state
    //       - ensure correct formatting for integer values
}
