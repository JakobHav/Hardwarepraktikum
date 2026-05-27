
// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#define GPIO 0x50000000
#define OUTSET (GPIO + 0x508UL)
#define IN (GPIO + 0x510UL)
#define OUTCLR (GPIO + 0x50CUL)
#define DIRSET (GPIO + 0x518UL)
#define DIRCLR (GPIO + 0x51CUL)

void setup() { ourPinMode(26, HIGH); }

void loop() {
  setP026(true);
  delay(1000);
  setP026(false);
  delay(1000);
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

#include <Arduino.h>


void setup() {
  setTimer1Freq();
}


void loop() {

}


void setTimer1Freq() {

}


void setBuzzerFreq() {

}


extern "C" void TIMER1_IRQHandler() {

}



