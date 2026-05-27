
// ------------------------------------------------------------
//  Task 6:
//      Use your code from Task 4 and Task 5, and make necessary changes. 
//      Implement the function playMelody() according to the exercise sheet.
//      The array is already filled with the right frequency for each note.
// ------------------------------------------------------------

#include <Arduino.h>
uint16_t durations[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t notes[10]     = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659};
volatile uint32_t tCount = 0;
volatile uint8_t melodyIdx = 0;


void setup() {

  playMelody();
}


void loop() {

}


void playMelody() {

}