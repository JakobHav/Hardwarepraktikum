
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
#include <U8g2lib.h>
#include <Wire.h>
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

const char *songs[] = {
    "Test:d=4,o=5,b=200:8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,"
    "8a,d6,8p,d6,8p,c6,8p,a.,8g,8a,8c6,8a,2c6,d6,b,a,g.,8p,g,2d6,"
    "2c6.,p,8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,8a,g6,b,c6,"
    "8p,b,8a,p,8g,8a,8c6,8a,2c6,d6,b,a,g,p,g,d6,c6",

    "GoodSong1:d=4,o=4,b=112:c,d#,f.,c,d#,8f#,f,p,c,d#,f.,d#,c",

    "GoodSong2:o=5,d=4,b=320:c,8d,8d,d,2d,c,c,c,c,8d#,8d#,2d#,d,d,d,c,"
    "8d,8d,d,2d,c,c,c,c,8d#,8d#,d#,2d#,d,c#,c,c6,1b.,g,f,1g.",

    "GoodSong3:o=5,d=8,b=112:d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,"
    "c6,e,d,d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,"
    "d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,a,d,e6,d,d6",

    "GoodSong4:o=5,d=8,b=125:16g,16g,a#.,16g,16p,16g,c6,g,f,"
    "4g,d6.,16g,16p,16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g,"
    "4p,16f6,d6,c6,a#,4g,a#.,16g,16p,16g,c6,g,f,4g,d6.,16g,16p,"
    "16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g",

    "GoodSong5:o=5,d=16,b=100:g,g,a,a,e,e,8g,g,g,a,a,e,e,8g,"
    "g,g,a,a,c6,c6,8b,8b,8a,8g,8f,f,f,g,g,d,d,8f,f,f,g,g,d,d,8f,f,"
    "f,g,g,a,b,8c6,8a,8g,8e,4c",

    "GoodSong6:o=5,d=8,b=140:g,e,4p,p,e,f,g,e6,p,e6,p,2c6,g,"
    "e,4p,p,e,f,e,g,p,g,p,2f,f,d,4p,p,d,e,f,g,e,4p,p,e,f#,e,d,g,p,"
    "e,f#,d,p,a,g.,16f#,g,a,g,f,e,d",

    "GoodSong7:o=5,d=8,b=63:a4,c,e,a,b,e,c,b,c6,e,c,c6,f#,d,"
    "a4,f#,e.,16c,a4,4e,c,a4,e,g4,a4,4a4",

    "GoodSong8:o=5,d=8,b=200:g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,"
    "f#,f,f,f#,g#,4c#,4d#,2f,2p,4g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#"
    ",f,f,f#,g#,4c#,4d#,2c#",

    "GoodSong9:o=4,d=8,b=125:c6,c6,a#5,c6,p,g5,p,g5,c6,f6,"
    "e6,c6,2p,c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6",

    "GoodSong10:o=5,d=8,b=160:c#6,a#,2p,a#,g#,f#,g#,a#,4c#"
    "6,a#,4c#6,d#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,b,2p,b,"
    "a#,g#,a#,b,4d#6,f#6,4d#6,4f6.,4d#6.,4c#6.,4b.,4a#,4g#",

    "GoodSong11:o=5,d=16,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,"
    "4a,4p,8p,c6,b,4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,"
    "a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b",
};
const uint8_t NUM_SONGS = sizeof(songs) / sizeof(songs[0]);
uint8_t songIdx = 0;

volatile uint8_t melodyIdx = 0;
volatile uint32_t tCount = 0;
uint16_t standardDuration = 4;
uint16_t standardOctave = 6;
uint16_t standardBPM = 63;

typedef struct {
  uint16_t duration_ms;
  uint16_t freq;
  uint16_t octave;
} Note;

Note *currentMelody = nullptr;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// define beforehand so the code knows it exists
Note parseRTTLNote(char *rt_note, uint16_t std_duration, uint16_t std_octave);
uint16_t freqFromNote(char note);

// ===================================
// Utility Functions
// ===================================

// writes analog speaker output
void writeSpeaker(bool output) {
  if (output) {
    NRF_P0->OUTSET = (1UL << SPK_BIT);
  } else {
    NRF_P0->OUTCLR = (1UL << SPK_BIT);
  }
}

// sets analog pin mode for P0 bit to output or input
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

// starts Timer1
void setTimer1Freq() {
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
  NRF_TIMER1->PRESCALER = 4; // 16Mhz/2^4~= 1MHz

  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  NVIC_EnableIRQ(TIMER1_IRQn);

  NRF_TIMER1->TASKS_START = 1;
}

// sets the frequency of Timer1 to produce the buzzer sound
void setBuzzerFreq(unsigned long freq) {
  if (freq >= 100 && freq <= 3000) {
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->CC[0] = round(1000000.0 / (2 * freq));
  } else {
    writeSpeaker(false);
  }
}

// enables or stops the Timer2 buzzer timer
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
// Display
// ============================================

// display a song by it's RTTL string on the OLED display
void showSong(const char *rtttl) {
  char name[32];
  const char *colon = strchr(rtttl, ':');
  size_t len = colon ? (size_t)(colon - rtttl) : strlen(rtttl);
  if (len >= sizeof(name))
    len = sizeof(name) - 1;
  strncpy(name, rtttl, len);
  name[len] = '\0';

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_12b_tr);
  u8g2.drawStr(0, 12, "Now playing:");
  u8g2.drawStr(0, 30, name);
  u8g2.setCursor(0, 48);
  u8g2.printf("%d / %d", songIdx + 1, NUM_SONGS);
  u8g2.sendBuffer();
}

// ============================================
// Melody Functions
// ============================================

// returns the adjusted frequency for a note
// respecting the octave
static uint16_t adjustedFreq(const Note &n) {
  int diff = (int)n.octave - 4;
  uint16_t f = n.freq;
  if (diff > 0)
    f <<= diff;
  else if (diff < 0)
    f >>= (-diff);
  return f;
}

// plays the melody stored in `melody`
void playRTTTL(Note *melody) {
  currentMelody = melody;
  melodyIdx = 0;

  if (melody[0].freq > 0)
    setBuzzerFreq(adjustedFreq(melody[0]));
  else
    writeSpeaker(false);

  setTimer2(true);
}

// ============================================
// Helper functions
// ============================================

// parses an RTTL note string into a Note struct
Note *melodyFromString(const char *mel) {
  uint32_t len = strlen(mel);
  char *copy = new char[len + 1];
  strcpy(copy, mel);

  // RTTL format: name:d=X,o=Y,b=Z:<duration><note><octave><dotted>
  strtok(copy, ":");
  char *defs = strtok(nullptr, ":");
  char *note_str = strtok(nullptr, "");

  // Parse defaults
  char *def_tok = strtok(defs, ",");
  while (def_tok) {
    if (def_tok[1] == '=') {
      int val = atoi(&def_tok[2]);
      if (def_tok[0] == 'd')
        standardDuration = val;
      else if (def_tok[0] == 'o')
        standardOctave = val;
      else if (def_tok[0] == 'b')
        standardBPM = val;
    }
    def_tok = strtok(nullptr, ",");
  }

  // Count comma-separated notes to size the array
  uint16_t count = 1;
  for (char *p = note_str; *p; p++)
    if (*p == ',')
      count++;

  Note *melody = new Note[count + 1]; // safeguard

  // Parse each note
  char *note_copy = new char[strlen(note_str) + 1];
  strcpy(note_copy, note_str);
  uint16_t i = 0;
  char *tok = strtok(note_copy, ",");
  while (tok) {
    melody[i++] = parseRTTLNote(tok, standardDuration, standardOctave);
    tok = strtok(nullptr, ",");
  }
  melody[i] = Note{0, 0, 0}; // safeguard

  delete[] note_copy;
  delete[] copy;
  return melody;
}

// returns the frequency of a note given its letter
uint16_t freqFromNote(char note) {
  uint8_t noteIndex = 0;
  for (int i = 0; i < 12; i++) {
    if (noteNames[i] == note)
      noteIndex = i;
  }
  return notes[noteIndex];
}

// parses an RTTL note string into a Note struct
Note parseRTTLNote(char *rt_note, uint16_t std_duration, uint16_t std_octave) {
  uint8_t idx = 0;
  uint16_t note_value = 0;

  // optional duration
  while (rt_note[idx] && isDigit(rt_note[idx]))
    note_value = note_value * 10 + (rt_note[idx++] - '0');
  if (note_value == 0)
    note_value = std_duration;

  // note letter
  char note_char = rt_note[idx++];

  // optional octave
  uint16_t octave = std_octave;
  if (rt_note[idx] && isDigit(rt_note[idx]))
    octave = rt_note[idx++] - '0';

  // optional dot
  bool dotted = (rt_note[idx] == '.');

  // save ms duration directly in note struct
  uint32_t base_ms = (60000UL / standardBPM) * 4 / note_value;
  uint16_t duration_ms =
      dotted ? (uint16_t)round(base_ms * 1.5) : (uint16_t)base_ms;

  uint16_t freq = (note_char == 'p') ? 0 : freqFromNote(note_char);
  return Note{duration_ms, freq, octave};
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }

// ============================================
// Setup + Loop
// ============================================

void setup() {
  pinModeP0(SPK_BIT, true);
  pinMode(D1, INPUT_PULLUP);

  u8g2.begin();

  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 3000)
    ;

  setTimer1Freq();

  Note *mel = melodyFromString(songs[songIdx]);
  playRTTTL(mel);
  showSong(songs[songIdx]);
}

void loop() {
  static bool lastButton = false;
  static unsigned long lastDebounce = 0;

  bool pressed = !digitalRead(D1);

  if (pressed && !lastButton && millis() - lastDebounce > 50) {
    lastDebounce = millis();

    setTimer2(false);
    writeSpeaker(false);
    delete[] currentMelody;
    currentMelody = nullptr;

    songIdx = (songIdx + 1) % NUM_SONGS;

    Note *mel = melodyFromString(songs[songIdx]);
    playRTTTL(mel);
    showSong(songs[songIdx]);
  }

  lastButton = pressed;
}

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

extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[1]) {
    NRF_TIMER2->EVENTS_COMPARE[1] = 0;

    if (tCount >= currentMelody[melodyIdx].duration_ms) {
      tCount = 0;
      melodyIdx++;

      if (currentMelody[melodyIdx].duration_ms == 0)
        melodyIdx = 0;

      if (currentMelody[melodyIdx].freq > 0)
        setBuzzerFreq(adjustedFreq(currentMelody[melodyIdx]));
      else
        writeSpeaker(false);
    }

    tCount++;
  }
}
