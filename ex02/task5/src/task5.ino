
// ------------------------------------------------------------
//  Task 5:
//      Write the body of setTimer2() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER2_IRQHandler().
// ------------------------------------------------------------

#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
volatile uint32_t tCount = 0;


void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Initializing Timer 2 (1ms interval)...");
}


void loop() {

}


extern "C" void TIMER2_IRQHandler() {

}


void setTimer2(bool enable) {

}
