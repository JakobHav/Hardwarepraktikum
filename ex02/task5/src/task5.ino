
// ------------------------------------------------------------
//  Task 5:
//      Write the body of setTimer2() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER2_IRQHandler().
// ------------------------------------------------------------

#include "Adafruit_USBD_CDC.h"
#include "delay.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include "variant.h"
#include "wiring_constants.h"
#include "wiring_digital.h"
#include <Arduino.h>
#include <cstdint>

#define GPIO 0x50000000

#define SPK_BIT 29
#define BUTTON_BIT 3

bool speaker_on;
bool button_on;

volatile uint32_t tCount;

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

bool readButton() { return !(NRF_P0->IN & (1UL << BUTTON_BIT)); }

// -------------------------------------------------------------------
// Setting Timer and Buzzer
// -------------------------------------------------------------------

// As seen in the previous tasks
// Starting Timer1, setting Modes etc. we decided we dont need to change
// prescaler for the different frequencies.
void setTimer1Freq() {
  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
  NRF_TIMER1->PRESCALER = 4; // 16Mhz/2^4~= 1MHz

  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  // Attatching interrupt
  NVIC_EnableIRQ(TIMER1_IRQn);

  NRF_TIMER1->TASKS_START = 1;
}

// setBuzzerFreq, writes in CC according to the ON-Time
// (antiproportional to 2*freq in microsec bec Clk ist running @1Mhz)
void setBuzzerFreq(unsigned long freq) {
  if (freq >= 100 && freq <= 3000) {

    NRF_TIMER1->CC[0] = round(1000000.0 / (2 * freq)); // e.g. 2092 for frew
  } else {
    writeSpeaker(false);
  }
}

// Starting Timer2, setting Modes etc. we decided we dont need to change
// prescaler for the different frequencies.
void setTimer2(bool enable) {
  if (enable) {
    NRF_TIMER2->TASKS_STOP = 1;
    NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
    NRF_TIMER2->PRESCALER = 4; // 16Mhz/2^4~= 1MHz

    NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Msk;
    NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE1_Msk;

    // Attatching interrupt
    NVIC_EnableIRQ(TIMER2_IRQn);

    NRF_TIMER2->CC[1] = 1000;

    tCount = 0; // Initializing tCount

    NRF_TIMER2->TASKS_START = 1;
  } else {
    NRF_TIMER2->TASKS_CLEAR = 1;
    NRF_TIMER2->TASKS_STOP = 1;
  }
}

// -------------------------------------------------------------------
// Setup and Loop
// -------------------------------------------------------------------
//
//
void setup() {
  pinMode(D1, INPUT_PULLUP);
  pinModeP0(SPK_BIT, true);

  button_on = false;

  Serial.begin(115200);
  while (!Serial)
    ;
  delay(500);

  // Starting timer2
  setTimer2(true);
}

void loop() {
  if (tCount >= 1000) {
    Serial.println(tCount); // Verifying Timer2 working.
    tCount = 0;
  }
}

// -------------------------------------------------------------------
// ISRs
// -------------------------------------------------------------------

extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    speaker_on = !speaker_on;
    writeSpeaker(speaker_on && !button_on);
  }
}

// Interrupt for Timer 2, increments tCount.
extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[1]) {
    NRF_TIMER2->EVENTS_COMPARE[1] = 0;

    tCount++;
  }
}
