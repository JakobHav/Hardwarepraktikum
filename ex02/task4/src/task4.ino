
// ------------------------------------------------------------
//  Task 4:
//      Make necessary changes to your code from Task 3. Provided tests
//      do not cover all cases.
// ------------------------------------------------------------

#include <Arduino.h>


void setup() {


}


void loop() {
// tests
  setBuzzerFreq(440);  //A4
  delay(300);
  setBuzzerFreq(554);  //C#5
  delay(300);
  setBuzzerFreq(659);  //E5
  delay(300);
  setBuzzerFreq(880);  //A5
  delay(500);
  setBuzzerFreq(5000); //out of range
  delay(3000);
  for (int f = 100; f <= 3000; f += 50) {
    setBuzzerFreq(f);
    delay(20);}
}


void setBuzzerFreq(uint32_t freq) {

}


extern "C" void TIMER1_IRQHandler() {

}


void ourPinMode(unsigned long pin, bool output) {
  if (output) {
    *(unsigned long *)DIRSET = (1UL << pin);
  } else {
    *(unsigned long *)DIRCLR = (1UL << pin);
  }
}

void ourDigitalWrite(unsigned long pin, bool high) {
  if (high) {
    *(unsigned long *)OUTSET = (1UL << pin);
  } else {
    *(unsigned long *)OUTCLR = (1UL << pin);
  }
}

bool ourDigitalRead(unsigned long pin) {
  return (*(unsigned long*) IN) & (1UL << pin);
  }


