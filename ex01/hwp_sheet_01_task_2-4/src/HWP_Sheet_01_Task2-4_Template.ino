
#include <Adafruit_TinyUSB.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <cstdint>

// Task 2 ii.) Constants — fill in the correct values from the datasheet
//             You can also write the answers to the questions here.
// ------------------------------------------------------------

#define SGP30_ADDR 0x58 // 7-bit I2C address of the SGP30

// Command codes (2 bytes each, MSB first — see datasheet )
#define CMD_INIT_MSB 0x20 //    first byte
#define CMD_INIT_LSB 0x03 //    second byte
#define CMD_MEAS_MSB 0x20
#define CMD_MEAS_LSB 0x08

// How many bytes does a measurement

// Display: air quality range for mapping CO2 to a percentage, you can change
// these to test more ranges
#define CO2_MIN 400  // ppm — clean outdoor air
#define CO2_MAX 2000 // ppm — poor indoor air quality

#define BOX_WIDTH 100 // ppm — clean outdoor air

#define MEASURE_INTERVAL_MS 1000UL

// ------------------------------------------------------------
//  Display constructor
// ------------------------------------------------------------
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

//  Task 2 iii.) — Helper functions
// ------------------------------------------------------------
//  Raw byte storage for the last SGP30 measurement.
//
// We use global variables here to keep the function signatures simple.
// The cleaner alternatives (pointers or a result struct) use C concepts not yet
// introduced. At this scale globals are fine; in a larger project you would
// avoid them.
//
//  The SGP30 sends each 16-bit value as two separate bytes:
//    MSB — most significant byte  (upper 8 bits of the value)
//    LSB — least significant byte (lower 8 bits of the value)
//  A third byte per value is a CRC checksum — we discard it.
// ------------------------------------------------------------
uint8_t raw_co2_msb, raw_co2_lsb;   // assign values with sgp30_read()
uint8_t raw_tvoc_msb, raw_tvoc_lsb; // assign values with sgp30_read()

void sgp30_cmd(uint8_t msb, uint8_t lsb) {
  // ------------------------------------------------------------
  //  sgp30_cmd : send a 2-byte command to the SGP30
  //
  //  The SGP30 expects all commands as two bytes (MSB first).
  //  Example:  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  // ------------------------------------------------------------
  // TODO: open a transmission to SGP30_ADDR,
  //       write msb, write lsb, close the transmission.

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
  // TODO: use Wire.requestFrom(SGP30_ADDR, n) to request n bytes.
  //       If the return value is not n, return false immediately.
  //       The expected return value is in the Datasheet.
  //       Read the n bytes in order, remember wire.read() can only
  //       read one byte at a time. Look out for CRC bytes,
  //       we don't need to store those.
  //
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

//  Task 3 — Display helper  (optional, but keeps loop() clean)

void display_values(uint16_t co2, uint16_t tvoc) {
  // ------------------------------------------------------------
  //  display_values : show co2 and tvoc on the OLED
  //
  // ------------------------------------------------------------
  // TODO (Task 3): set cursor, print co2 and tvoc values.
  // TODO (Task 4): map co2 to pct (0-100), draw a filled bar with
  //                u8g2.drawBox(x, y, width, height).
  //                Bar width  = map(pct, 0, 100, 0, 128)
  //                Remember: constrain pct to [0, 100] before mapping.
}

unsigned long init_time_ms = 0;

void setup() {
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 3000)
    ;
  // Wait for USB Serial connection
  //     Task 2 i.): I2C scanner
  Wire.begin();

  for (uint8_t addr = 8; addr <= 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.println(addr, HEX);
    }
  }

  //     Task 2 iv.): Initialise SGP30
  // TODO: send the init command, wait for initialization
  //       and print out a message.

  Serial.println("Initializing SGP30...");
  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  delay(10);
  init_time_ms = millis();

  // --- Task 3 i.): Simple display use ---
  // TODO: initialize display, set a font, display "Hardware Praktikum 2026",
  //       and push it to the screen.
  u8g2.begin();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_12b_tr);
  u8g2.drawStr(0, 10, "Hardware-Praktikum");
  u8g2.drawStr(0, 20, "Starting up sensor... :)");
  u8g2.sendBuffer();
  delay(15000);
}

unsigned long last_measure_ms = 0;

void loop() {

  // --- Task 2 iv.): Send measure command and read response ---
  // --- Task 3 ii.): Print the sgp30 values on the display
  //                  in addition to the Serial monitor
  unsigned long current = millis();

  if (current - last_measure_ms >= MEASURE_INTERVAL_MS) {
    sgp30_cmd(CMD_MEAS_MSB, CMD_MEAS_LSB);
    delay(12);
    sgp30_read(6);

    uint16_t co2 = to_uint16(raw_co2_msb, raw_co2_lsb);
    uint16_t tvoc = to_uint16(raw_tvoc_msb, raw_tvoc_lsb);

    Serial.printf("eCO2: %d - ", co2);
    Serial.printf("TVOC: %d \n", tvoc);

    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "Hardware-Praktikum");
    u8g2.setCursor(0, 20);
    u8g2.printf("eCO2: %d", co2);
    u8g2.setCursor(0, 40);
    u8g2.printf("TVOC: %d", tvoc);
    u8g2.sendBuffer();

    uint8_t width_co2 = constrain(map(co2, CO2_MIN, CO2_MAX, 0, 100), 0, 100);
    uint8_t width_tvoc = constrain(map(tvoc, CO2_MIN, CO2_MAX, 0, 100), 0, 100);

    u8g2.drawBox(0, 30, width_co2, 8);
    u8g2.drawBox(0, 50, width_co2, 8);

    last_measure_ms = current;
  }

  // --- Task 4: Map CO2 to a percentage ---
  // TODO: use map() to scale co2 from raw values to 0-100%.
  //       Then use constrain() to make sure the percetange
  //       doesnt go outside 0-100.
}
