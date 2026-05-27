
// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#include "delay.h"
#include "nrf52840_bitfields.h"
#include "variant.h"
#include "wiring_constants.h"
#include "wiring_digital.h"
#define GPIO 0x50000000
#define OUTSET (GPIO + 0x508UL)
#define IN (GPIO + 0x510UL)
#define OUTCLR (GPIO + 0x50CUL)
#define DIRSET (GPIO + 0x518UL)
#define DIRCLR (GPIO + 0x51CUL)
#define SPKPORT 0x313F8

#define SPK D3
#define C6 1046

bool speaker_on;

#include <Arduino.h>

void setup() {
  pinMode(SPK, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {;}
}


void loop() {
    digitalWrite(SPK, HIGH);
    delayMicroseconds(1000000/C6/2);
    digitalWrite(SPK, LOW);
    delayMicroseconds(1000000/C6/2);

    // Serial.printf("%X\n", &D3);
    Serial.printf("%d\n", round(32.768 / 10) - 1);
}


void setTimer1Freq() {
    NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
    NRF_TIMER1->PRESCALER = 4; // ~= 1MHz
    NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
    NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

}


void setBuzzerFreq(unsigned long freq) {

    NRF_TIMER1->CC[0] = 2 * freq; // e.g. 2092 for frew
  
}


extern "C" void TIMER1_IRQHandler() {
    if (NRF_TIMER1->EVENTS_COMPARE[0]) {
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
        speaker_on = !speaker_on;
        digitalWrite(SPK, speaker_on);
    }
}
