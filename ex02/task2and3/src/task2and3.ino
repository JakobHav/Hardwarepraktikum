
// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#include "delay.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include "variant.h"
#include "wiring_constants.h"
#include "wiring_digital.h"

#define GPIO 0x50000000

#define SPK_BIT 29
#define BUTTON_BIT 3

#define C6 1046

bool speaker_on;
bool button_on;

#include <Arduino.h>

// -------------------------------------------------------------------
// Utility Functions
// -------------------------------------------------------------------

void writeSpeaker(bool output) {
  if (output) {
    NRF_P0->OUTSET = (1UL << SPK_BIT);
  } else {
    NRF_P0->OUTCLR = (1UL << SPK_BIT);
  }
}

void pinModeP0(unsigned long bit, bool output) {
  if (output) {
    NRF_P0->DIRSET = (1UL << bit);
  } else {
    NRF_P0->DIRCLR = (1UL << bit);
  }
}

// RETURNS true if button is pressed
bool readButton() { return !(NRF_P0->IN & (1UL << BUTTON_BIT)); }

void setBuzzerFreq(unsigned long freq) {

  NRF_TIMER1->CC[0] = round(1000000.0 / (2 * freq)); // e.g. 2092 for frew
}

void setTimer1Freq() {
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
  NRF_TIMER1->PRESCALER = 4; // ~= 1MHz

  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  NVIC_EnableIRQ(TIMER1_IRQn);

  NRF_TIMER1->TASKS_START = 1;
}

void setup() {
  pinMode(D1, INPUT_PULLUP);
  // pinModeP0(BUTTON_BIT, false);
  pinModeP0(SPK_BIT, true);

  setTimer1Freq();
  setBuzzerFreq(1046);

  // Serial.begin(115200);
}

void loop() { button_on = readButton(); }

extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    speaker_on = !speaker_on;
    writeSpeaker(speaker_on && button_on);
  }
}
