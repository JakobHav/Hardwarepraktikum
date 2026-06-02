
// ------------------------------------------------------------
//  Task 7:
//      Use your code from Task 6, adding a parser function to
//      go over any given string and create the array needed to play
//      it.
//      You are free to use or discard any helper functions and add
//      any helper functions you need for parsing.
// ------------------------------------------------------------

#include "WCharacter.h"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <cstdint>
#include <cstring>
#include <string.h>

#define GPIO 0x50000000

#define SPK_BIT 29
#define BUTTON_BIT 3

bool speaker_on;

const char noteNames[] = {'c', 'C', 'd', 'D', 'e', 'f',
                          'F', 'g', 'G', 'a', 'A', 'b'};
const uint16_t notes[] = {262, 277, 294, 311, 330, 349,
                          370, 392, 415, 440, 466, 494};
char buffer[] = "Test:d=4,o=5,b=200:8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,"
                "8a,d6,8p,d6,8p,c6,8p,a.,8g,8a,8c6,8a,2c6,d6,b,a,g.,8p,g,2d6,"
                "2c6.,p,8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,8a,g6,b,c6,"
                "8p,b,8a,p,8g,8a,8c6,8a,2c6,d6,b,a,g,p,g,d6,c6";
volatile uint8_t melodyIdx = 0;
volatile uint32_t tCount = 0;
uint16_t standardDuration = 4;
uint16_t standardOctave = 6;
uint16_t standardBPM = 63;

typedef struct {
  uint16_t duration;
  uint16_t freq;
  uint16_t octave;
} Note;

// ===================================
// Utility Functions
// ===================================

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

// ============================================
// Timer / Buzzer
// ============================================

void setTimer1Freq() {
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
  NRF_TIMER1->PRESCALER = 4; // 16Mhz/2^4~= 1MHz

  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  NVIC_EnableIRQ(TIMER1_IRQn);

  NRF_TIMER1->TASKS_START = 1;
}

void setBuzzerFreq(unsigned long freq) {
  if (freq >= 100 && freq <= 3000) {
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->CC[0] = round(1000000.0 / (2 * freq)); // e.g. 2092 for frew
  } else {
    writeSpeaker(false);
  }
}

void setTimer2(bool enable) {
  if (enable) {
    NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
    NRF_TIMER2->PRESCALER = 4; // 16Mhz/2^4~= 1MHz

    NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Msk;
    NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE1_Msk;

    NVIC_EnableIRQ(TIMER2_IRQn);

    NRF_TIMER2->CC[1] = 1000;

    tCount = 0;

    NRF_TIMER2->TASKS_START = 1;
  } else {
    NRF_TIMER2->TASKS_CLEAR = 1;
    NRF_TIMER2->TASKS_STOP = 1;
  }
}

// ============================================
// Melody Functions
// ============================================

void playRTTTL(Note *melody) {}

// ============================================
// Helper functions
// ============================================

Note *melodyFromString(char *mel) {
  uint32_t len = strlen(mel);

  Note *melody = new Note[len];
  char *buf = new char[len];
  uint8_t bufIdx = 0;

  uint8_t counter = 0;

  char *start = strtok(mel, ":");

  while (uint16_t i = 0; i < strlen(start); i++) {
    Serial.println(*(start+1));
    start = strtok(nullptr, ":");
  }


  delete[] buf;
  return melody;
}

uint16_t freqFromNote(char note) {
  uint8_t noteIndex = 0;
  for (int i = 0; i < 12; i++) {
    if (noteNames[i] == note)
      noteIndex = i;
  }

  return noteNames[noteIndex];
}

uint16_t makelonger(uint16_t duration, bool longer) {
  return longer ? (uint16_t)round(duration * 1.5) : duration;
}

Note parseRTTLNote(char *rt_note, uint16_t std_duration, uint16_t std_octave) {
  size_t len = strlen(rt_note);
  bool longer = rt_note[len - 1] == '.';
  len = longer ? len - 1 : len;

  switch (len) {
  case 1:
    return Note{std_duration, freqFromNote(rt_note[0]), std_octave};
    break;
  case 2:
    // either 8a or d6
    if (isDigit(rt_note[0]))
      return Note{makelonger(rt_note[0], longer), freqFromNote(rt_note[1]),
                  std_octave};
    else
      return Note{makelonger(std_duration, longer), freqFromNote(rt_note[0]),
                  rt_note[1]};
    break;
  case 3:
    return Note{makelonger(rt_note[0], longer), freqFromNote(rt_note[1]),
                rt_note[2]};
    break;
  }
}

uint16_t str2uint(char *buf, uint16_t *idx) {
  uint16_t result;
  while (isDigit(buf[*idx])) {
    result = result * 10 + (buf[*idx] - '0');
  }
  return result;
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }

// ============================================
// Setup + Loop
// ============================================

void setup() {
  String song0 = "";
  String song1 = "GoodSong1:d=4,o=4,b=112:c,d#,f.,c,d#,8f#,f,p,c,d#,f.,d#,c";
  String song2 =
      "GoodSong2:o=5,d=4,b=320,b=320:c,8d,8d,d,2d,c,c,c,c,8d#,8d#,2d#,d,d,d,c,"
      "8d,8d,d,2d,c,c,c,c,8d#,8d#,d#,2d#,d,c#,c,c6,1b.,g,f,1g.";
  String song3 = "GoodSong3:o=5,d=8,b=112,b=112:d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,"
                 "c6,e,d,d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,"
                 "d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,a,d,e6,d,d6";
  String song4 = "GoodSong4:o=5,d=8,b=125,b=125:16g,16g,a#.,16g,16p,16g,c6,g,f,"
                 "4g,d6.,16g,16p,16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g,"
                 "4p,16f6,d6,c6,a#,4g,a#.,16g,16p,16g,c6,g,f,4g,d6.,16g,16p,"
                 "16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g";
  String song5 = "GoodSong5:o=5,d=16,b=100,b=100:g,g,a,a,e,e,8g,g,g,a,a,e,e,8g,"
                 "g,g,a,a,c6,c6,8b,8b,8a,8g,8f,f,f,g,g,d,d,8f,f,f,g,g,d,d,8f,f,"
                 "f,g,g,a,b,8c6,8a,8g,8e,4c";
  String song6 = "GoodSong6:o=5,d=8,b=140,b=140:g,e,4p,p,e,f,g,e6,p,e6,p,2c6,g,"
                 "e,4p,p,e,f,e,g,p,g,p,2f,f,d,4p,p,d,e,f,g,e,4p,p,e,f#,e,d,g,p,"
                 "e,f#,d,p,a,g.,16f#,g,a,g,f,e,d";
  String song7 = "GoodSong7:o=5,d=8,b=63,b=63:a4,c,e,a,b,e,c,b,c6,e,c,c6,f#,d,"
                 "a4,f#,e.,16c,a4,4e,c,a4,e,g4,a4,4a4";
  String song8 = "GoodSong8:o=5,d=8,b=200,b=200:g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,"
                 "f#,f,f,f#,g#,4c#,4d#,2f,2p,4g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#"
                 ",f,f,f#,g#,4c#,4d#,2c#";
  String song9 = "GoodSong9:o=4,d=8,b=125,b=125:c6,c6,a#5,c6,p,g5,p,g5,c6,f6,"
                 "e6,c6,2p,c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6";
  String song10 = "GoodSong10:o=5,d=8,b=160,b=160:c#6,a#,2p,a#,g#,f#,g#,a#,4c#"
                  "6,a#,4c#6,d#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,b,2p,b,"
                  "a#,g#,a#,b,4d#6,f#6,4d#6,4f6.,4d#6.,4c#6.,4b.,4a#,4g#";
  String song11 = "GoodSong11:o=5,d=16,b=125,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,"
                  "4a,4p,8p,c6,b,4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,"
                  "a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b";

  Serial.begin(115200);

  unsigned long start = millis();
  while (!Serial && millis() - start < 3000)
    ;

  Note *mel = melodyFromString(buffer);
  playRTTTL(mel);
}

void loop() {Note *mel = melodyFromString(buffer);
playRTTTL(mel);}

// ============================================
// Interrupt Service Routines
// ============================================

extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    speaker_on = !speaker_on;
    writeSpeaker(speaker_on);
  }
}

extern "C" void TIMER2_IRQHandler() {}
