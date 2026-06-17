// Task 4: Smart Plant Monitoring Station
#include <Adafruit_SGP30.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <bluefruit.h>

// --- Hardware Configuration ---
#define DHT11_PIN D7
#define DHTTYPE DHT11
// Buzzer on P0.29 (no Dx alias on this board — use NRF_P0 register directly)
#define BUZZ_BIT 29
#define LIGHT_PIN A0
#define LED_PIN LED_BUILTIN

// --- Light calibration (Ex3.1 style: low raw = bright, high raw = dark) ---
// LIGHT_RAW_BRIGHT is the raw ADC reading under your brightest indoor condition
// (e.g. desk lamp close to sensor). Increase this if lightPct stays stuck near
// 0.
#define LIGHT_RAW_BRIGHT 300
#define LIGHT_RAW_DARK 3000

// --- Timing Constants ---
#define LIGHT_INTERVAL 500UL
#define DHT_INTERVAL 2000UL
#define SGP_INTERVAL 1000UL
#define SCORE_INTERVAL 1000UL
#define DISPLAY_INTERVAL 500UL // ~2 Hz
#define BLE_INTERVAL 1000UL
#define WARMUP_DURATION 1000UL

// --- FSM ---
enum SystemState { STATE_INIT, STATE_HEALTHY, STATE_ATTENTION, STATE_STRESSED };
SystemState currentState = STATE_INIT;
SystemState prevState = STATE_INIT;

// --- Objects ---
DHT DHT11_Sensor(DHT11_PIN, DHTTYPE);
Adafruit_SGP30 sgp;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
BLEService plantService("19b10000-e8f2-537e-4f6c-d104768a1214");
BLECharacteristic plantCharacteristic("19b10001-e8f2-537e-4f6c-d104768a1214",
                                      BLERead | BLENotify, 64);

// --- Timing ---
unsigned long lastLightSample = 0;
unsigned long lastDhtSample = 0;
unsigned long lastSgpSample = 0;
unsigned long lastScoreUpdate = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastBleTransmission = 0;
unsigned long lastLedToggle = 0;
unsigned long lastBuzzToggle = 0;
unsigned long systemStartTime = 0;

// --- Sensor data ---
float temp = 25.0f;
float humid = 50.0f;
uint16_t eCO2 = 400;
uint16_t TVOC = 0;
int lightPct = 50;
int healthScore = 0;

// --- LED / buzzer state ---
bool ledIsOn = false;
bool buzzIsOn = false;
bool buzzerActive = false;

// --- Helpers ---

const char *stateToString(SystemState s) {
  switch (s) {
  case STATE_INIT:
    return "INIT";
  case STATE_HEALTHY:
    return "HEALTHY";
  case STATE_ATTENTION:
    return "ATTENTION";
  case STATE_STRESSED:
    return "STRESSED";
  default:
    return "UNKNOWN";
  }
}

// Maps raw ADC reading to 0–100 % using Ex3.1 calibration values
int lightNormalize(int raw) {
  raw = constrain(raw, LIGHT_RAW_BRIGHT, LIGHT_RAW_DARK);
  return (int)map(raw, LIGHT_RAW_BRIGHT, LIGHT_RAW_DARK, 0, 100);
}

void buzzWrite(bool on) {
  if (on) {
    NRF_P0->OUTSET = (1UL << BUZZ_BIT);
  } else {
    NRF_P0->OUTCLR = (1UL << BUZZ_BIT);
  }
}

void setTimer1Freq() {
  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
  NRF_TIMER1->PRESCALER = 4; // 16Mhz/2^4~= 1MHz

  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  // Attatching interrupt
  NVIC_EnableIRQ(TIMER1_IRQn);

  unsigned long freq = 1482;
  NRF_TIMER1->CC[0] = round(1000000.0 / (2 * freq)); // e.g. 2092 for frew

  NRF_TIMER1->TASKS_START = 1;
}

// ---

void setup() {
  Serial.begin(115200);

  // Pins
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // Buzzer via NRF_P0 register (P0.29)
  NRF_P0->DIRSET = (1UL << BUZZ_BIT);
  buzzWrite(false);
  analogReadResolution(12);

  setTimer1Freq();

  // Sensors
  DHT11_Sensor.begin();
  sgp.begin();
  sgp.IAQinit();

  // Display
  u8g2.begin();

  // BLE
  Bluefruit.begin();
  Bluefruit.setName("PlantMonitor");

  plantService.begin();
  plantCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  plantCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  plantCharacteristic.setMaxLen(64);
  plantCharacteristic.begin();

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addService(plantService);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.start(0);

  systemStartTime = millis();
  lastLightSample = systemStartTime;
  lastDhtSample = systemStartTime;
  lastSgpSample = systemStartTime;
  lastScoreUpdate = systemStartTime;
  lastDisplayUpdate = systemStartTime;
  lastBleTransmission = systemStartTime;
  lastLedToggle = systemStartTime;
  lastBuzzToggle = systemStartTime;
}

void loop() {
  unsigned long now = millis();

  // --- i) Asynchronous sensor acquisition ---

  // Light: every 500 ms
  if (now - lastLightSample >= LIGHT_INTERVAL) {
    lastLightSample = now;
    lightPct = lightNormalize(analogRead(LIGHT_PIN));
  }

  // DHT11: every 2 s
  // BLE advertising events (~1 ms every 20 ms) can corrupt the 5 ms DHT
  // single-wire read. If the first attempt returns NaN, wait 10 ms (past the
  // current BLE slot) and retry once with force=true.
  if (now - lastDhtSample >= DHT_INTERVAL) {
    lastDhtSample = now;
    float t = DHT11_Sensor.readTemperature();
    float h = DHT11_Sensor.readHumidity();
    if (isnan(t) || isnan(h)) {
      delay(10);
      t = DHT11_Sensor.readTemperature(false, true);
      h = DHT11_Sensor.readHumidity(true);
    }
    if (!isnan(t))
      temp = t;
    if (!isnan(h))
      humid = h;
  }

  // SGP30: every 1 s
  if (now - lastSgpSample >= SGP_INTERVAL) {
    lastSgpSample = now;
    if (sgp.IAQmeasure()) {
      eCO2 = sgp.eCO2;
      TVOC = sgp.TVOC;
    }
  }

  // --- ii) Warm-up (STATE_INIT for 30 s) ---
  bool inWarmUp = (now - systemStartTime < WARMUP_DURATION);
  if (inWarmUp) {
    currentState = STATE_INIT;
  }

  // --- iii) Health scoring & state transitions (every 1 s after warm-up) ---
  if (!inWarmUp && (now - lastScoreUpdate >= SCORE_INTERVAL)) {
    lastScoreUpdate = now;

    healthScore = 0;
    if (lightPct >= 25 && lightPct <= 90)
      healthScore += 25; // Ex3.1 range
    if (temp >= 18.0f && temp <= 30.0f)
      healthScore += 25;
    if (humid >= 30.0f && humid <= 75.0f)
      healthScore += 25;
    if (eCO2 < 1200)
      healthScore += 25;

    // Critical override: eCO2 > 2200 ppm forces STRESSED regardless of score
    if (eCO2 > 2200) {
      currentState = STATE_STRESSED;
    } else if (healthScore >= 75) {
      currentState = STATE_HEALTHY;
    } else if (healthScore >= 50) {
      currentState = STATE_ATTENTION;
    } else {
      currentState = STATE_STRESSED;
    }
  }

  // Reset LED toggle timer on state change so new rate takes effect immediately
  if (currentState != prevState) {
    lastLedToggle = now;
    ledIsOn = (currentState == STATE_HEALTHY);
    digitalWrite(LED_PIN, ledIsOn ? HIGH : LOW);
    prevState = currentState;
  }

  // --- iv) OLED display (~2 Hz, avoid unnecessary redraws) ---
  if (now - lastDisplayUpdate >= DISPLAY_INTERVAL) {
    lastDisplayUpdate = now;
    u8g2.clearBuffer();

    if (currentState == STATE_INIT) {
      u8g2.setFont(u8g2_font_8x13B_tr);
      u8g2.drawStr(4, 36, "Warming Up...");
    } else {
      char line[24];
      u8g2.setFont(u8g2_font_7x13B_tr);
      u8g2.drawStr(0, 12, stateToString(currentState));

      u8g2.setFont(u8g2_font_6x10_tr);
      snprintf(line, sizeof(line), "T: %.1f C", temp);
      u8g2.drawStr(0, 28, line);

      snprintf(line, sizeof(line), "H: %.0f %%", humid);
      u8g2.drawStr(0, 40, line);

      snprintf(line, sizeof(line), "L: %d %%", lightPct);
      u8g2.drawStr(0, 52, line);

      snprintf(line, sizeof(line), "CO2: %u ppm", eCO2);
      u8g2.drawStr(0, 63, line);
    }

    u8g2.sendBuffer();
  }

  // --- v) LED feedback (non-blocking) ---
  switch (currentState) {
  case STATE_INIT:
    digitalWrite(LED_PIN, LOW);
    break;
  case STATE_HEALTHY:
    digitalWrite(LED_PIN, HIGH);
    break;
  case STATE_ATTENTION:
    if (now - lastLedToggle >= 500UL) {
      lastLedToggle = now;
      ledIsOn = !ledIsOn;
      digitalWrite(LED_PIN, ledIsOn ? HIGH : LOW);
    }
    break;
  case STATE_STRESSED:
    if (now - lastLedToggle >= 250UL) {
      lastLedToggle = now;
      ledIsOn = !ledIsOn;
      digitalWrite(LED_PIN, ledIsOn ? HIGH : LOW);
    }
    break;
  }

  // Buzzer alarm (non-blocking pulsed beep, 200 ms on / 300 ms off)
  // Uses direct NRF_P0 register access (buzzer on P0.29, no Dx alias)
  if (eCO2 > 2200) {
    if (buzzIsOn && now - lastBuzzToggle >= 1000UL) {
      buzzIsOn = false;
      lastBuzzToggle = now;
      buzzWrite(false);
    } else if (!buzzIsOn && now - lastBuzzToggle >= 500UL) {
      buzzIsOn = true;
      lastBuzzToggle = now;
      buzzWrite(true);
    }
  } else {
    if (buzzerActive) {
      buzzerActive = false;
      buzzIsOn = false;
      buzzWrite(false);
    }
  }
  if (eCO2 > 2200 && !buzzerActive) {
    buzzerActive = true;
    buzzIsOn = true;
    lastBuzzToggle = now;
    buzzWrite(true);
  }

  // --- vi) BLE telemetry (1 Hz) ---
  if (now - lastBleTransmission >= BLE_INTERVAL) {
    lastBleTransmission = now;
    char buf[64];
    snprintf(buf, sizeof(buf), "T=%.1f H=%.0f L=%d C=%u S=%s", temp, humid,
             lightPct, eCO2, stateToString(currentState));
    if (Bluefruit.connected()) {
      plantCharacteristic.notify((uint8_t *)buf, strlen(buf));
    }
  }
}

extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    speaker_on = !speaker_on;
    writeSpeaker(speaker_on && !button_on);
  }
}
