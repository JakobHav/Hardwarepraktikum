// ------------------------------------------------------------
//  Task 4 (no change here as we already worked with different frequencies)
// ------------------------------------------------------------

#include "delay.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include "variant.h"
#include "wiring_constants.h"
#include "wiring_digital.h"
#include <ratio>

#define GPIO 0x50000000

#define SPK_BIT 29
#define BUTTON_BIT 3

bool speaker_on;
bool timer_stopped;
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

// -------------------------------------------------------------------
// Timer and Buzzer
// -------------------------------------------------------------------

void stopTimer() {
  writeSpeaker(false);
  timer_stopped = true;
  NRF_TIMER1->TASKS_STOP = 1;
}

// setBuzzerFreq, writes in CC according to the ON-Time
// (antiproportional to 2*freq in microsec bec Clk ist running @1Mhz)
void setBuzzerFreq(unsigned long freq) {
  if (freq >= 100 && freq <= 3000) {
    setTimer1Freq();
    NRF_TIMER1->CC[0] = round(1000000.0 / (2 * freq));
    timer_stopped = false;
  } else {
    stopTimer();
  }
}

// Startung Timer, setting Modes etc. we decided we dont need to change
// prescaler for the different frequencies.
void setTimer1Freq() {
  NRF_TIMER1->TASKS_STOP = 1;

  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
  NRF_TIMER1->PRESCALER = 4; // ~= 1MHz

  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  NVIC_EnableIRQ(TIMER1_IRQn);

  NRF_TIMER1->TASKS_START = 1;
}

// -------------------------------------------------------------------
// Setup and Loop
// -------------------------------------------------------------------

void setup() {
  // Pin Modes for button and speaker
  pinMode(D1, INPUT_PULLUP);
  pinModeP0(SPK_BIT, true);

  timer_stopped = true;
}

int freq = 100;
void loop() {
  // Button acts as a start / stop mechanism
  if (readButton()) {
    if (timer_stopped) {
      freq += 100;
      setBuzzerFreq(freq);
      delay(200); // Debounce Delay
    } else {
      stopTimer();
      delay(200); // Debounce Delay
    }
  }
}

// -------------------------------------------------------------------
// Interrupt to controll the Speaker
// -------------------------------------------------------------------

extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    speaker_on = !speaker_on;
    writeSpeaker(speaker_on);
  }
}
