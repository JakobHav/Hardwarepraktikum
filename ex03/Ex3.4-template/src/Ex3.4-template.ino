// Task 4: Smart Plant Monitoring Station

#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_SGP30.h>
#include <U8g2lib.h>
#include <DHT.h>

// --- Hardware Configuration ---
// TODO: define pins (DHT, light sensor, LED, buzzer)

// --- System Constants ---
// TODO: define timing constants for:
// - sensor sampling
// - display refresh (~2 Hz)
// - BLE transmission (1 Hz)
// - warm-up duration (30 s)

// --- Objects ---
// TODO: initialize display, sensors, BLE service and characteristic

// --- FSM ---
enum SystemState {
    STATE_INIT,
    STATE_HEALTHY,
    STATE_ATTENTION,
    STATE_STRESSED
};

SystemState currentState = STATE_INIT;

// TODO: define timing variables for asynchronous operation

// TODO: define variables for sensor data storage

// TODO: helper function(s), e.g.:
// - state to string conversion
// - value normalization (light)

void setup() {
    Serial.begin(115200);

    // TODO: initialize hardware (pins, display, sensors)

    // TODO: initialize BLE and start advertising

    // TODO: store system start time (for warm-up)
}

void loop() {
    unsigned long now = millis();

    // TODO: maintain BLE stack (if required)

    // i) TODO: asynchronous sensor acquisition (light, DHT, SGP30)

    // iii) TODO: warm-up handling (STATE_INIT for 30 s)

    // iii) TODO: compute health score (0–100)

    // iii) TODO: implement state transitions (including critical override)

    // iv) TODO: update OLED display (~2 Hz)

    // v) TODO: implement LED and buzzer behavior (non-blocking)

    // vi) TODO: send BLE telemetry (formatted string, 1 Hz)
}