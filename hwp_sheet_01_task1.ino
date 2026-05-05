#include <Adafruit_TinyUSB.h>
#define BLINK_INTERVAL_MS   1000UL  // 1 second on, 1 second off → 1 Hz
#define CYCLES_PER_COLOR    10

// ------------------------------------------------------------
//  Global variables for Task 1 ii) and beyond
//  Use the smallest type that fits:
//    unsigned long — required for millis() values (32-bit)
//    uint8_t       — sufficient for a cycle/colour counter (8-bit, max 255)
// ------------------------------------------------------------
unsigned long last_blink_ms = 0;  // timestamp of the last LED toggle
bool          led_on        = false; // current LED state

// TODO (Bonus): add a counter for completed blink cycles
//               and a variable to track the current colour
const int colors[3] = {LED_RED, LED_GREEN, LED_BLUE};

uint8_t current_color = 0;
uint8_t cycle_count = 0;


void setup() {
  Serial.begin(115200);

  unsigned long start = millis();
  while (!Serial && millis() - start < 3000);

  Serial.println("Setup running");

  Serial.print("LED_RED = "); Serial.println(LED_RED);
  Serial.print("LED_GREEN = "); Serial.println(LED_GREEN);
  Serial.print("LED_BLUE = "); Serial.println(LED_BLUE);

  // Set LED pins as outputs.
  // pinMode(pin, OUTPUT) configures a pin for digital output
  // Start with all LEDs off (active-low → HIGH = off)
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}


void loop() {

  // ----------------------------------------------------------
  //  Task 1 i) — Blink using delay()
  //  Comment this section out once you move on to Task 1 ii).
  // ----------------------------------------------------------

  // digitalWrite(LED_RED, LOW);
  // delay(BLINK_INTERVAL_MS);
  // digitalWrite(LED_RED, HIGH);
  // delay(BLINK_INTERVAL_MS);


  // ----------------------------------------------------------
  //  Task 1 ii) — Blink using millis()  (comment out i) first)
  //
  // TODO: get the current time with millis().
  // TODO: check whether BLINK_INTERVAL_MS has elapsed since last_blink_ms.
  // TODO: if yes — update last_blink_ms, toggle led_on,
  //               and write the correct HIGH/LOW to LED_RED.

  unsigned long current = millis();

  if (current - last_blink_ms >= BLINK_INTERVAL_MS) {
    led_on = !led_on;
    last_blink_ms = current;

    digitalWrite(colors[current_color], led_on ? LOW : HIGH);

    if (!led_on) {
      cycle_count++;

      if (cycle_count >= CYCLES_PER_COLOR) {
        cycle_count = 0;

        digitalWrite(colors[current_color], HIGH);

        current_color = (current_color + 1) % 3;

        Serial.print("Switching to color index ");
        Serial.println(current_color);
      }
    }
  }


  // ----------------------------------------------------------
  //  Task 1 iii) — Answer as a comment
  // ----------------------------------------------------------

  // The advantage of using the method from II) consists of asynchronous execution of code, using the delay(ms) function does not allow execution of code during the delay period.

  // ----------------------------------------------------------
  //  Bonus — Colour cycling after 10 blink cycles
  // ----------------------------------------------------------
  // TODO: count completed blink cycles (one cycle = on + off).
  //       Every 10 cycles, switch to the next colour:
  //         red → green → blue → red → ...
  //       Turn off all LEDs before switching, then turn on only
  //       the new active colour.
  //
  // completed above :)
}
