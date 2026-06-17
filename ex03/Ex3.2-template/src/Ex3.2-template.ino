// Task 2: Robust Temperature and Humidity Monitoring

#include <DHT.h>
#include <cstddef>
#include <math.h>
#include <sys/_types.h>

#define a 17.62f
#define b 243.12f

// --- Configuration ---
#define DHT11_PIN D7
#define DHTTYPE DHT11

// --- Objects ---
DHT DHT11_Sensor(DHT11_PIN, DHTTYPE);

// --- Timing ---
unsigned long lastSample = 0;

// sampling interval 2 seconds
#define sampleInterval 2000UL

// --- State Variables ---

float temp;
float humid;

size_t fails = 0;
size_t fail_thresh = 6;

unsigned long start;

// --- Computation ---
float computeDewPoint(float tempC, float relHum) {
  // Calculating dew point via Magnus formula using natural logarithm
  float gamma = log(relHum / 100.0f) + (a * tempC) / (b + tempC);

  return (b * gamma) / (a - gamma);
}

// -------------------------------------------------------------------
// Main Logic
// -------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    ;

  DHT11_Sensor.begin();
  start = millis();
}

void loop() {
  if (millis() - start >= sampleInterval) {
    start = millis();

    // reading temperature and humidity
    float humid_temp = DHT11_Sensor.readHumidity();
    float temp_temp = DHT11_Sensor.readTemperature();

    // isnan did not work for some reaseon ???
    if (!isnormal(temp_temp) || !isnormal(temp_temp)) {
      fails++;

      // If threshold is reached, printing
      if (fails >= fail_thresh) {
        Serial.print("[WARN] DHT failures: ");
        Serial.println(fails);
      }

    } else {

      // Updating global values with the not NaN Values
      temp = temp_temp;
      humid = humid_temp;

      fails = 0;
    }

    // Pretty print
    Serial.print("T: ");
    Serial.print(temp, 1);
    Serial.print("°C    ");

    Serial.print("RH: ");
    Serial.print(humid, 1);
    Serial.print("%    ");

    Serial.print("dewPoint: ");
    Serial.print(computeDewPoint(temp, humid), 1);
    Serial.println("°C    ");
  }
}
