# 1 "/var/folders/1w/ncpb_8bs1m5_713wt5691vn80000gn/T/tmp_88ppi25"
#include <Arduino.h>
# 1 "/Users/jakobhaverkamp/Documents/uni/hardwarepraktikum/Hardwarepraktikum/ex02/task5/src/task5.ino"
# 9 "/Users/jakobhaverkamp/Documents/uni/hardwarepraktikum/Hardwarepraktikum/ex02/task5/src/task5.ino"
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

#define C6 1046

bool speaker_on;
bool button_on;

volatile uint32_t tCount;
void writeSpeaker(bool output);
void pinModeP0(unsigned long bit, bool output);
bool readButton();
void setTimer1Freq();
void setBuzzerFreq(unsigned long freq);
void setTimer2(bool enable);
void setup();
void loop();
#line 34 "/Users/jakobhaverkamp/Documents/uni/hardwarepraktikum/Hardwarepraktikum/ex02/task5/src/task5.ino"
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





void setTimer1Freq() {
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
  NRF_TIMER1->PRESCALER = 4;

  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  NVIC_EnableIRQ(TIMER1_IRQn);

  NRF_TIMER1->TASKS_START = 1;
}

void setBuzzerFreq(unsigned long freq) {
  if (freq >= 100 && freq <= 3000) {

    NRF_TIMER1->CC[0] = round(1000000.0 / (2 * freq));
  } else {
    writeSpeaker(false);
  }
}

void setTimer2(bool enable) {
  if (enable) {
    NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
    NRF_TIMER2->PRESCALER = 4;

    NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
    NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

    NVIC_EnableIRQ(TIMER2_IRQn);

    NRF_TIMER2->CC[1] = 1000;

    tCount = 0;

    NRF_TIMER2->TASKS_START = 1;
  } else {
    NRF_TIMER2->TASKS_CLEAR = 1;
    NRF_TIMER2->TASKS_STOP = 1;
  }
}






void setup() {
  pinMode(D1, INPUT_PULLUP);
  pinModeP0(SPK_BIT, true);



  button_on = false;

  Serial.begin(115200);
  delay(2000);
  setTimer2(true);
}

void loop() {
  if (tCount >= 1000) {
    Serial.println(tCount);
    tCount = 0;
  }
  Serial.print(tCount);
}





extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    speaker_on = !speaker_on;
    writeSpeaker(speaker_on && !button_on);
  }
}

extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[1]) {
    NRF_TIMER2->EVENTS_COMPARE[1] = 0;

    tCount++;
  }
}