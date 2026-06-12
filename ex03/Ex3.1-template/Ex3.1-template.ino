// Task 1: Environmental Light Categorization
// Complete the implementation according to the task description.

// Defining necessary constants (e.g., calibration range, thresholds,
// timing)

#define dark 50UL
#define bright 3500UL
#define depth_12bit 2

#define SAADC 0x40007000
#define RESOLUTION_OFFSET 0x5F0

#define lowThresh 30
#define highThresh 70

// Categorizing the normalized value
const char *categorize(int normalized) {
  if (normalized <= lowThresh) {

    return "LOW";
  } else if (normalized < highThresh) {

    return "MEDIUM";
  } else {

    return "HIGH";
  }
}

void setup() {
  // Initializing Serial communication
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    ;

  // Configureing ADC resolution to 12-bit

  (char *)(SAADC + RESOLUTION_OFFSET)->depth_12bit;
}

void loop() {
  // Non-blockingly sampleing every 500 ms
  unsigned long start = millis();
  if (millis() - start >= 500) {

    // Reading raw value from light sensor
    unsigned long value = analogRead(1);

    // Clamping the raw value to a calibrated range
    clamp(value, dark, bright);

    // Normalizing the value to a 0–100 scale
    unsigned long normalized = map(value, dark, bright, 0, 100);

    // Determining the category using the categorize() function
    const char *cat = categorize(normalized);

    // Printing raw value, normalized value, and category to Serial

    Serial.print("R=");
    Serial.print(raw);
    Serial.print(", N=");
    Serial.print(normalized);
    Serial.print(", C=");
    Serial.println(cat);
    start = millis();
  }
}
