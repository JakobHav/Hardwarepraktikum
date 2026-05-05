
#include <Adafruit_TinyUSB.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <cstdint>

#define SGP30_ADDR 0x58 // 7-bit I2C address of the SGP30

#define CMD_INIT_MSB 0x20 //    first byte
#define CMD_INIT_LSB 0x03 //    second byte
#define CMD_MEAS_MSB 0x20
#define CMD_MEAS_LSB 0x08

// Measurement contains 3 Byte, Data MSB, Data LSB and CRC

#define CO2_MIN 400  // ppm — clean outdoor air
#define CO2_MAX 2000 // ppm — poor indoor air quality

#define BOX_WIDTH 100

#define MEASURE_INTERVAL_MS 200UL

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

uint8_t raw_co2_msb, raw_co2_lsb;   // assign values with sgp30_read()
uint8_t raw_tvoc_msb, raw_tvoc_lsb; // assign values with sgp30_read()

// Initial Time
unsigned long init_time_ms = 0;
unsigned long last_measure_ms = 0;

// Array and pointer for Task 4
uint8_t storage_ptr = 0;
uint8_t co2_storage[128] = {60};

void sgp30_cmd(uint8_t msb, uint8_t lsb) {
  // ------------------------------------------------------------
  //  sgp30_cmd : send a 2-byte command to the SGP30
  //
  //  The SGP30 expects all commands as two bytes (MSB first).
  //  Example:  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  // ------------------------------------------------------------

  Wire.beginTransmission(SGP30_ADDR);
  Wire.write(msb);
  Wire.write(lsb);
  Wire.endTransmission();
}

bool sgp30_read(uint8_t n) {
  // ------------------------------------------------------------
  //  sgp30_read : read one measurement(n bytes) from the SGP30.
  //
  //  Call sgp30_cmd() first, wait max measurement duration,
  //  then call this function.
  //  The 4 raw bytes are stored in the global variables above.
  //  The two CRC bytes are read from the bus and discarded.
  //
  //  Returns true if all 6 bytes were received, false on error.
  // ------------------------------------------------------------

  if (Wire.requestFrom(SGP30_ADDR, n) != n)
    return false;

  raw_co2_msb = Wire.read();
  raw_co2_lsb = Wire.read();
  Wire.read();
  raw_tvoc_msb = Wire.read();
  raw_tvoc_lsb = Wire.read();

  Wire.read();

  return true;
}

uint16_t to_uint16(uint8_t msb, uint8_t lsb) {
  // ------------------------------------------------------------
  //  to_uint16 : combine two bytes into one 16-bit value.
  //
  //  A sensor value like CO2 = 450 ppm cannot fit in a single
  //  byte (max 255). The sensor splits it across two bytes:
  //    MSB holds the upper half: 450 >> 8  = 1   (0x01)
  //    LSB holds the lower half: 450 & 0xFF = 194 (0xC2)
  //
  //  To reconstruct the original value:
  //    shift MSB left by 8 bits  →  0x01 becomes 0x0100 (= 256)
  //    OR with LSB               →  0x0100 | 0xC2 = 0x01C2 (= 450)
  //
  //  The cast to uint16_t before shifting is necessary because
  //  uint8_t would overflow when shifted — always cast first.
  //
  // ------------------------------------------------------------
  return ((uint16_t)msb << 8) | lsb;
}

void display_values(uint16_t co2, uint16_t tvoc) {
  // ------------------------------------------------------------
  //  display_values : show co2 and tvoc on the OLED
  //
  // ------------------------------------------------------------

  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Hardware-Praktikum");
  u8g2.setCursor(0, 20);
  u8g2.printf("eCO2: %d", co2);
  u8g2.setCursor(0, 40);
  u8g2.printf("TVOC: %d", tvoc);

  uint8_t co2_percent = constrain(map(co2, CO2_MIN, CO2_MAX, 0, 100), 0, 100);
  uint8_t co2_width = map(co2_percent, 0, 100, 0, 128);

  // drawing the box
  u8g2.drawBox(0, 22, co2_width, 8);

  // For the creative part we decided on a graph. We add a new value to the
  // array every cycle Then we go through the array and draw a line between each
  // measuring point and its right neighbor

  co2_storage[storage_ptr] = map(co2_percent, 0, 100, 60, 41);
  storage_ptr = (storage_ptr + 1) % 128;

  for (uint8_t x = 0; x < 127; x++) {
    uint8_t val = co2_storage[(x + storage_ptr) % 128];
    uint8_t next_val = co2_storage[(x + storage_ptr + 1) % 128];

    u8g2.drawLine(x, val, x + 1, next_val);
  }

  u8g2.sendBuffer();
}

void setup() {

  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 3000)
    ;

  // I2C Scanner

  Wire.begin();

  for (uint8_t addr = 8; addr <= 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.println(addr, HEX);
    }
  }

  // Initializing Sensor

  Serial.println("Initializing SGP30...");
  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  delay(10);
  init_time_ms = millis();

  // Initializing Display & setting font

  u8g2.begin();
  u8g2.setFont(u8g2_font_t0_12b_tr);

  // Display initial Text
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Hardware-Praktikum");
  u8g2.drawStr(0, 20, "Starting up sensor... :)");
  u8g2.sendBuffer();

  // Initializing our Storage Array for Task 4 to 60 (lowest pixel value) :)
  for (uint8_t x = 0; x <= 127; x++) {
    co2_storage[x] = 60;
  }

  delay(15000);
}

void loop() {
  unsigned long current = millis();

  // Measuring every 200ms
  if (current - last_measure_ms >= MEASURE_INTERVAL_MS) {
    sgp30_cmd(CMD_MEAS_MSB, CMD_MEAS_LSB);
    delay(12);
    sgp30_read(6);

    // Converting CO2 and TVOC to 16 bit int
    uint16_t co2 = to_uint16(raw_co2_msb, raw_co2_lsb);
    uint16_t tvoc = to_uint16(raw_tvoc_msb, raw_tvoc_lsb);

    // Printing to Serial
    Serial.printf("eCO2: %d - ", co2);
    Serial.printf("TVOC: %d \n", tvoc);

    display_values(co2, tvoc);

    last_measure_ms = current;
  }
}
