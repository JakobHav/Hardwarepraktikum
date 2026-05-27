
// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#include "delay.h"
#include "wiring_digital.h"
#define GPIO 0x50000000
#define OUTSET (GPIO + 0x508UL)
#define IN (GPIO + 0x510UL)
#define OUTCLR (GPIO + 0x50CUL)
#define DIRSET (GPIO + 0x518UL)
#define DIRCLR (GPIO + 0x51CUL)

#define SPK D3
#define C6 1046

#include <Arduino.h>

void setup() {
  pinMode(SPK, OUTPUT);
}


void loop() {
    digitalWrite(SPK, HIGH);
    delayMicroseconds(1000000/C6/2);
    digitalWrite(SPK, LOW);
    delayMicroseconds(1000000/C6/2);

}


void setTimer1Freq() {

}


void setBuzzerFreq() {

}


extern "C" void TIMER1_IRQHandler() {

}

