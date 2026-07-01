/*
 * Exercise 04: Wearable Context Recognition with IMU
 * Author: Roozbeh Ghazavi
 * Year: 2026
 * Board: XIAO nRF52840 Sense
 * Sensor: LSM6DS3
 *
 * Implement ALL TODO sections.
 */

#include <Arduino.h>
#include "LSM6DS3.h"
#include "Wire.h"

// =====================
// Part E: BLE Library
// =====================
#include <ArduinoBLE.h>

LSM6DS3 myIMU(I2C_MODE, 0x6A);

// Constants
#define CONVERT_G_TO_MS2 9.80665f
#define FREQUENCY_HZ 50
#define INTERVAL_MS (1000 / (FREQUENCY_HZ + 1))

static unsigned long last_interval_ms = 0;

// =====================
// Part C: Accelerometer Buffer (Z-axis)
// =====================
#define WINDOW_SIZE 10

float az_buffer[WINDOW_SIZE];
int buffer_index = 0;
bool buffer_full = false;


// =====================
// Part C & D: Last Detected Gestures
// =====================
// TODO: Create variables to store last detected gestures
// - lastDetectedGesture: from accelerometer FSM
// - lastDynamicGesture: from gyroscope
String lastDetectedGesture = "NONE";
String lastDynamicGesture = "NONE";



// =====================
// Part E: BLE Objects
// =====================
// TODO: Create BLE service and characteristic
// Example:
// BLEService imuService("180C");
// BLECharacteristic imuCharacteristic("2A56", BLERead | BLENotify, 100);


// =====================
// Part B: Orientation Detection
// =====================
String detectOrientation(float ax, float ay, float az) {

  // TODO: Detect device orientation from accelerometer
  // Use thresholds on ay and az axes

  return "UNKNOWN";
}


// =====================
// Part C: Accelerometer-based Gesture Detection
// =====================
String detectGestureWindow() {

  // TODO: Detect SUPINATION/PRONATION from Z-axis acceleration buffer
  // Analyze min/max range and motion direction

  return "NONE";
}


// =====================
// Part D: Gyroscope-based Dynamic Gesture Detection
// =====================
String detectDynamicGesture(float gyrX, float gyrY, float gyrZ) {

  // TODO: Detect 6 gestures using gyroscope
  // Gestures: TILT_LEFT, TILT_RIGHT, MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT
  // Return strongest gesture (highest magnitude)

  return "NONE";
}


// =====================
// Part C (Bonus): Gesture State Machine (FSM)
// =====================
enum State {
  IDLE,
  MOVING,
  DETECTED
};

State currentState = IDLE;

String detectGestureFSM() {

  // TODO: Implement FSM for robust gesture detection
  // Transitions: IDLE → MOVING → DETECTED → IDLE
  // Store detected gesture in lastDetectedGesture

  return "NONE";
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (myIMU.begin() != 0) {
    Serial.println("IMU initialization failed!");
    while (1);
  }

  Serial.println("IMU initialized.");

  // =====================
  // Part E: BLE Setup
  // =====================
  // TODO: Initialize BLE, add service/characteristic, start advertising
}


void loop() {
  if (millis() > last_interval_ms + INTERVAL_MS) {
    last_interval_ms = millis();

    // =====================
    // Part A: IMU Data Acquisition
    // =====================
    // TODO: Read accelerometer (ax, ay, az) from myIMU
    // TODO: Convert accelerometer from G to m/s² using CONVERT_G_TO_MS2
    // TODO: Read gyroscope (gyrX, gyrY, gyrZ) from myIMU

    float ax;  // TODO
    float ay;  // TODO
    float az;  // TODO

    float gyrX;  // TODO
    float gyrY;  // TODO
    float gyrZ;  // TODO


    // =====================
    // Part C: Accelerometer Buffer Management
    // =====================
    az_buffer[buffer_index] = az;
    buffer_index++;

    if (buffer_index >= WINDOW_SIZE) {
      buffer_index = 0;
      buffer_full = true;
    }


    // =====================
    // Part B: Orientation Detection
    // =====================
    String orientation = detectOrientation(ax, ay, az);


    // =====================
    // Part C: Accelerometer-based Gesture Detection
    // =====================
    // TODO: Call detectGestureWindow() or detectGestureFSM()
    // TODO: If gesture detected (not "NONE"), store in lastDetectedGesture
    String detectedGesture = "NONE";  // TODO: Replace with actual detection

    // =====================
    // Part D: Gyroscope-based Dynamic Gesture Detection
    // =====================
    // TODO: Call detectDynamicGesture()
    // TODO: If gesture detected (not "NONE"), store in lastDynamicGesture
    String dynamicGesture = "NONE";  // TODO: Replace with actual detection

    // =====================
    // Serial Output (USB)
    // =====================
    Serial.print("ax: "); Serial.print(ax);
    Serial.print(" | ay: "); Serial.print(ay);
    Serial.print(" | az: "); Serial.print(az);

    Serial.print(" | gyrX: "); Serial.print(gyrX);
    Serial.print(" | gyrY: "); Serial.print(gyrY);
    Serial.print(" | gyrZ: "); Serial.print(gyrZ);

    Serial.print(" | Orientation: ");
    Serial.print(orientation);

    Serial.print(" | Accelerometer Gesture: ");
    Serial.print(lastDetectedGesture);

    Serial.print(" | Gyro Gesture: ");
    Serial.println(lastDynamicGesture);


    // =====================
    // Part E: Bluetooth Low Energy (BLE) Communication
    // =====================
    // TODO: Format and send data via BLE:
    // - Accelerometer readings (ax, ay, az)
    // - Gyroscope readings (gyrX, gyrY, gyrZ)
    // - Orientation detection result
    // - Gesture detection results (FSM and Gyro)
  }
}
