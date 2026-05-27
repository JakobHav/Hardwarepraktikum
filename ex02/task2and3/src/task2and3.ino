
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

#include <Arduino.h>

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


void setup() {
}

void playTone(int tone, int duration) {
    for (long i = 0; i < duration * 1000L; i+= tone * 2) {
        digitalWrite(A3, HIGH);
        delayMicroseconds(tone);
        digitalWrite(D3, LOW);
        delayMicroseconds(tone);
    }
}

void loop() {
    delay(1000);
    playTone(1014, 10000);
}


void setTimer1Freq() {

}


void setBuzzerFreq() {

}


extern "C" void TIMER1_IRQHandler() {

}


// -------------------------------------------------


int speakerPin = D3;
int length = 28; // the number of notes
char notes[] = "GGAGcB GGAGdc GGxecBA yyecdc";
int beats[] = { 2, 2, 8, 8, 8, 16, 1, 2, 2, 8, 8, 8, 16, 1, 2, 2, 8, 8, 8, 8, 16, 1, 2, 2, 8, 8, 8, 16 };
int tempo = 150;
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = {'C', 'D', 'E', 'F', 'G', 'A', 'B',
                  'c', 'd', 'e', 'f', 'g', 'a', 'b',
                  'x', 'y'
                 };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014,
                  956,  834,  765,  593,  468,  346,  224,
                  655 , 715
                };
  int SPEE = 5;

  // play the tone corresponding to the note name

  for (int i = 0; i < 16; i++) {
    if (names[i] == note) {
      int newduration = duration / SPEE;
      playTone(tones[i], newduration);
    }
  }
}

// void setup() {
//   pinMode(speakerPin, OUTPUT);
// }

// void loop() {
//   for (int i = 0; i < length; i++) {
//     if (notes[i] == ' ') {
//       delay(beats[i] * tempo); // rest
//     } else {
//       playNote(notes[i], beats[i] * tempo);
//     }
//     // pause between notes
//     delay(tempo);
//   }
// }
