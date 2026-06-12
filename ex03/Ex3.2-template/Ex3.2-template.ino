// Task 2: Robust Temperature and Humidity Monitoring

#include <DHT.h>
#include <math.h>

// --- Configuration ---
// TODO: define sensor pin and type (DHT11)

// --- Objects ---
// TODO: create DHT sensor instance

// --- Timing ---
unsigned long lastSample = 0;
// TODO: define sampling interval (2 seconds)

// --- State Variables ---
// TODO: store last valid temperature and humidity
// TODO: maintain a failure counter

// --- Computation ---
float computeDewPoint(float tempC, float relHum) {
    // TODO: implement Magnus formula using natural logarithm
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    // TODO: initialize sensor
}

void loop() {
    unsigned long now = millis();

    // TODO: implement non-blocking sampling (2 s)

    // TODO: read temperature and humidity

    // TODO: handle invalid readings (NaN)
    // - update failure counter (reset on success)
    // - reuse last valid values if needed

    // TODO: compute dew point, print formatted output. If failure count exceeds threshold print a warning. 

}