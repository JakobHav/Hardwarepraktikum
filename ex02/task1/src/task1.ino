#define GPIO 0x50000000
#define OUTSET (GPIO + 0x508UL)
#define OUTCLR (GPIO + 0x50CUL)
#define DIRSET (GPIO + 0x518UL)
#define DIRCLR (GPIO + 0x51CUL)

// Bit an Position pin im DIRCLR (für Input) oder in
// DIRSET (für Output) Register auf 1 setzen
void ourPinMode(unsigned long pin, bool output) {
  if (output) {
    *(unsigned long *)DIRSET = (1UL << pin);
  } else {
    *(unsigned long *)DIRCLR = (1Ul << pin);
  }
}

// Bit an Position pin im OUTSET (für HIGH) oder in
// OUTCLR (für LOW) Register auf 1 setzen
void setP026(bool high) {
  if (high) {
    *(unsigned long *)OUTSET = (1UL << 26);
  } else {
    *(unsigned long *)OUTCLR = (1UL << 26);
  }
}

// Pinmode im Setup setzen
void setup() { ourPinMode(26, HIGH); }

// Jede Sekunde Pin Abwchselnd LOW und HIGH setzen
void loop() {
  setP026(true);
  delay(1000);
  setP026(false);
  delay(1000);
}
