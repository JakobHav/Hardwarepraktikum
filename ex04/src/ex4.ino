/*
 * Exercise 04: Wearable Context Recognition with IMU
 * Author: Roozbeh Ghazavi
 * Year: 2026
 * Board: XIAO nRF52840 Sense
 * Sensor: LSM6DS3
 *
 * Implement ALL TODO sections.
 */
#include "LSM6DS3.h"
#include "Wire.h"
#include <Arduino.h>

// =====================
// Part E: BLE Library
// =====================
#include <ArduinoBLE.h>
#include <utility/ATT.h>
#include <cmath>

LSM6DS3 myIMU(I2C_MODE, 0x6A);

// Constants
#define CONVERT_G_TO_MS2 9.80665f
#define FREQUENCY_HZ 50
#define INTERVAL_MS (1000 / (FREQUENCY_HZ + 1))
#define GRAV_THRESH 0.9F
#define MOVEMENT_THRESH 1.8F
#define GYRO_THRESH 100.0F // Deg/s

static unsigned long last_interval_ms = 0;

// =====================
// Part C: Accelerometer Buffer (Z-axis)
// =====================
#define WINDOW_SIZE 10
#define MIN_SAMPLES 5

float az_buffer[WINDOW_SIZE];
int buffer_index = 0;
bool buffer_full = false;

// =====================
// Part C & D: Last Detected Gestures
// =====================
// Create variables to store last detected gestures
// - lastDetectedGesture: from accelerometer FSM
// - lastDynamicGesture: from gyroscope
String lastDetectedGesture = "NONE";
String lastDynamicGesture = "NONE";

// =====================
// Part E: BLE Objects
// =====================
// Create BLE service and characteristic
// Example:
// BLEService imuService("180C");
// BLECharacteristic imuCharacteristic("2A56", BLERead | BLENotify, 100);

BLEService imuService("F48217BF-94D7-4CF9-9CDA-9333615D4137");
BLECharacteristic imuCharacteristic("46807E7A-F299-4B3B-B9E6-E062D944AF8D", BLERead | BLENotify, 150);

// =====================
// Part B: Orientation Detection
// =====================
String detectOrientation(float ax, float ay, float az) {

  // Detect device orientation from accelerometer
  // Use thresholds on ay and az axes

  if (az > GRAV_THRESH) {
    return "FACE UP";
  } else if (az < -GRAV_THRESH) {
    return "FACE DOWN";
  }

  if (ay > GRAV_THRESH) {
    return "RIGHT SIDE";
  } else if (ay < -GRAV_THRESH) {
    return "LEFT SIDE";
  }

  return "UNKNOWN";
}

// =====================
// Part C: Accelerometer-based Gesture Detection
// =====================
String detectGestureWindow() {

  // Detect SUPINATION/PRONATION from Z-axis acceleration buffer
  // Analyze min/max range and motion direction

  if (buffer_index < MIN_SAMPLES)
    return "NONE";

  float min_val = az_buffer[0];
  float max_val = az_buffer[0];

  for (int i = 1; i < WINDOW_SIZE; i++) {
    if (az_buffer[i] < min_val)
      min_val = az_buffer[i];
    if (az_buffer[i] > max_val)
      max_val = az_buffer[i];
  }

  if ((max_val - min_val) < MOVEMENT_THRESH) {
    return "NONE";
  }

  return (az_buffer[WINDOW_SIZE - 1] > az_buffer[0]) ? "PRONATION"
                                                     : "SUPINATION";
}

// =====================
// Part D: Gyroscope-based Dynamic Gesture Detection
// =====================
String detectDynamicGesture(float gyrX, float gyrY, float gyrZ) {

  // Detect 6 gestures using gyroscope
  // Gestures: TILT_LEFT, TILT_RIGHT, MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT
  // Return strongest gesture (highest magnitude)

  float absX = fabs(gyrX);
  float absY = fabs(gyrY);
  float absZ = fabs(gyrZ);

  if (absX < GYRO_THRESH && absY < GYRO_THRESH && absZ < GYRO_THRESH) {
    return "NONE";
  }

  if (absX >= absY && absX >= absZ) {
    // X axis dominant: tilt left/right
    return (gyrX > 0) ? "TILT_RIGHT" : "TILT_LEFT";
  } else if (absY >= absX && absY >= absZ) {
    // Y axis dominant: move up/down
    return (gyrY > 0) ? "MOVE_UP" : "MOVE_DOWN";
  } else {
    // Z axis dominant: move left/right
    return (gyrZ > 0) ? "MOVE_RIGHT" : "MOVE_LEFT";
  }
}

// =====================
// Part C (Bonus): Gesture State Machine (FSM)
// =====================
enum State { IDLE, MOVING, DETECTED };

State currentState = IDLE;

String detectGestureFSM() {

  // TODO: Implement FSM for robust gesture detection
  // Transitions: IDLE → MOVING → DETECTED → IDLE
  // Store detected gesture in lastDetectedGesture

  return "NONE";
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  if (myIMU.begin() != 0) {
    Serial.println("IMU initialization failed!");
    while (1)
      ;
  }

  Serial.println("IMU initialized.");

  // =====================
  // Part E: BLE Setup
  // =====================

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed");
    while (1)
      ;
  }
  ATT.setMaxMtu(200);
  BLE.setLocalName("seeed studio XIAO nRF52");
  BLE.setAdvertisedService(imuService);

  imuService.addCharacteristic(imuCharacteristic);
  BLE.addService(imuService);
  BLE.advertise();

  Serial.println("BLE advertising started");
}

void loop() {
  BLE.poll();

  if (millis() > last_interval_ms + INTERVAL_MS) {
    last_interval_ms = millis();

    // =====================
    // Part A: IMU Data Acquisition
    // =====================
    // Read accelerometer (ax, ay, az) from myIMU
    // Convert accelerometer from G to m/s² using CONVERT_G_TO_MS2
    // Read gyroscope (gyrX, gyrY, gyrZ) from myIMU

    float ax = myIMU.readFloatAccelX();
    float ay = myIMU.readFloatAccelY();
    float az = myIMU.readFloatAccelZ();

    float gyrX = myIMU.readFloatGyroX();
    float gyrY = myIMU.readFloatGyroY();
    float gyrZ = myIMU.readFloatGyroZ();

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
    // Call detectGestureWindow() or detectGestureFSM()
    // If gesture detected (not "NONE"), store in lastDetectedGesture

    String detectedGesture =
        detectGestureWindow(); // Replace with actual detection
    if (detectedGesture != "NONE") {
      lastDetectedGesture = detectedGesture;
    }

    // =====================
    // Part D: Gyroscope-based Dynamic Gesture Detection
    // =====================
    // Call detectDynamicGesture()
    // If gesture detected (not "NONE"), store in lastDynamicGesture
    String dynamicGesture = detectDynamicGesture(gyrX, gyrY, gyrZ);
    if (dynamicGesture != "NONE") {
      lastDynamicGesture = dynamicGesture;
    }

    // =====================
    // Serial Output (USB)
    // =====================
    Serial.print("ax: ");
    Serial.print(ax);
    Serial.print(" | ay: ");
    Serial.print(ay);
    Serial.print(" | az: ");
    Serial.print(az);

    Serial.print(" | gyrX: ");
    Serial.print(gyrX);
    Serial.print(" | gyrY: ");
    Serial.print(gyrY);
    Serial.print(" | gyrZ: ");
    Serial.print(gyrZ);

    Serial.print(" | Orientation: ");
    Serial.print(orientation);

    Serial.print(" | Accelerometer Gesture: ");
    Serial.print(lastDetectedGesture);

    Serial.print(" | Gyro Gesture: ");
    Serial.println(lastDynamicGesture);

    // =====================
    // Part E: Bluetooth Low Energy (BLE) Communication
    // =====================
    // Format and send data via BLE:
    // - Accelerometer readings (ax, ay, az)
    // - Gyroscope readings (gyrX, gyrY, gyrZ)
    // - Orientation detection result
    // - Gesture detection results (FSM and Gyro)

    String bleData = "ax:" + String(ax, 2) + "|ay:" + String(ay, 2) +
                     "|az:" + String(az, 2) + "|gyrX:" + String(gyrX, 2) +
                     "|gyrY:" + String(gyrY, 2) + "|gyrZ:" + String(gyrZ, 2) +
                     "|Orientation:" + orientation +
                     "|FSM:" + lastDetectedGesture +
                     "|Gyro:" + lastDynamicGesture;

    imuCharacteristic.writeValue(bleData.c_str());
  }
}
