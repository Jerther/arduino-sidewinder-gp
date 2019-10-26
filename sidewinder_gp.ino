// PINS
const int SW_CLOCK   = 2;
const int SW_TRIGGER = 3;
const int SW_DATA    = 4;

// DATA
const byte SW_DATA_MASK = 1 << SW_DATA;
volatile byte swBitPosition = 0;
volatile bool swButton0 = false;
volatile bool swButton1 = false;
volatile bool swButton2 = false;
volatile bool swButton3 = false;
volatile bool swButton4 = false;
volatile bool swButton5 = false;
volatile bool swButton6 = false;
volatile bool swButton7 = false;
volatile bool swButton8 = false;
volatile bool swButton9 = false;
volatile bool swButton10 = false;
volatile bool swButton11 = false;
volatile bool swButton12 = false;
volatile bool swButton13 = false;
volatile bool swParity = false;
bool triggering = true;

void setup() {
  Serial.begin(115200);
  setupPins();
}

void setupPins() {
  pinMode(SW_DATA, INPUT_PULLUP);
  pinMode(SW_CLOCK, INPUT_PULLUP);
  pinMode(SW_TRIGGER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(SW_CLOCK), readSW, FALLING);
}

void loop() {
  triggering = true;
  if(checkParity())
    onValidData();
  
  resetData();
  
  digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);  // this is slow
  
  digitalWrite(SW_TRIGGER, LOW);        // Request packet
  delayMicroseconds(250);               // Wait at least 225us for clock to start and become stable.
  digitalWrite(SW_TRIGGER, HIGH);       // Reset
  triggering = false;
  // The delay here sets the trigger frequency and depends on the time onValidData() takes.
  // The faster it is, the greater the delay must be here.
  // The frequency should not exceed 1 KHz. The gamepad starts to behave erratically above that.
  // There is no way to know the value other than measuring or guessing.
  delayMicroseconds(100);
}

bool checkParity() {
  int swSetBitsCount = 0;
  if(swButton0) swSetBitsCount++;
  if(swButton1) swSetBitsCount++;
  if(swButton2) swSetBitsCount++;
  if(swButton3) swSetBitsCount++;
  if(swButton4) swSetBitsCount++;
  if(swButton5) swSetBitsCount++;
  if(swButton6) swSetBitsCount++;
  if(swButton7) swSetBitsCount++;
  if(swButton8) swSetBitsCount++;
  if(swButton9) swSetBitsCount++;
  if(swButton10) swSetBitsCount++;
  if(swButton11) swSetBitsCount++;
  if(swButton12) swSetBitsCount++;
  if(swButton13) swSetBitsCount++;
  bool calculatedParity = swSetBitsCount % 2 == 0;
  return swParity == calculatedParity;
}

void readSW() {
  // Time critical procedure! This has to be fast enough to keep up with the SW clock!
  if(triggering)  // Ignore noise when triggering
    return;
  bool value = (PIND & SW_DATA_MASK) == B00000000;  // digitalRead is too slow!
  switch(swBitPosition) {
    case 0: swButton0 = value; break;
    case 1: swButton1 = value; break;
    case 2: swButton2 = value; break;
    case 3: swButton3 = value; break;
    case 4: swButton4 = value; break;
    case 5: swButton5 = value; break;
    case 6: swButton6 = value; break;
    case 7: swButton7 = value; break;
    case 8: swButton8 = value; break;
    case 9: swButton9 = value; break;
    case 10: swButton10 = value; break;
    case 11: swButton11 = value; break;
    case 12: swButton12 = value; break;
    case 13: swButton13 = value; break;
    case 14: swParity = value; break;
  }
  ++swBitPosition;
}

void onValidData() {
  // Send values to USB host
  Serial.print(swButton0);
  Serial.print(swButton1);
  Serial.print(swButton2);
  Serial.print(swButton3);
  Serial.print(swButton4);
  Serial.print(swButton5);
  Serial.print(swButton6);
  Serial.print(swButton7);
  Serial.print(swButton8);
  Serial.print(swButton9);
  Serial.print(swButton10);
  Serial.print(swButton11);
  Serial.print(swButton12);
  Serial.println(swButton13);
}

void resetData() {
  swParity = false;
  swButton0 = false;
  swButton1 = false;
  swButton2 = false;
  swButton3 = false;
  swButton4 = false;
  swButton5 = false;
  swButton6 = false;
  swButton7 = false;
  swButton8 = false;
  swButton9 = false;
  swButton10 = false;
  swButton11 = false;
  swButton12 = false;
  swButton13 = false;
  swBitPosition = 0;
}
