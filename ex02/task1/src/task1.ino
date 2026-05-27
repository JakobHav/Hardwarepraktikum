#define GPIO 0x50000000
#define OUTSET (GPIO + 0x508UL)
#define OUTCLR (GPIO + 0x50CUL)
#define DIRSET (GPIO + 0x518UL)
#define DIRCLR (GPIO + 0x51CUL)

void ourPinMode(unsigned long pin, bool high) {
  if (high) {
    *(unsigned long *)DIRSET = (1 << pin);
  } else {
    *(unsigned long *)DIRCLR = (1 << pin);
  }
}

void setP026(bool high) {
  if (high) {
    *(unsigned long *)OUTSET = (1 << 26);
  } else {
    *(unsigned long *)OUTCLR = (1 << 26);
  }
}

void setup() { ourPinMode(26, HIGH); }

void loop() {
  setP026(true);
  delay(1000);
  setP026(false);
  delay(1000);
}
