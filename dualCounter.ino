#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();

#include <Bounce2.h>

#define buttonOne 2
#define buttonTwo 3
#define debounceTime 5

#define MAX 99
#define MIN 0

byte counts[] = {0, 0};
byte count = 0;
boolean mode = false;
int old_state = 3;
int state = 3;

Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();

void displayCount();

void setup() {
  pinMode(buttonOne, INPUT_PULLUP);
  pinMode(buttonTwo, INPUT_PULLUP);

  debouncer1.attach(buttonOne);
  debouncer1.interval(debounceTime);

  debouncer2.attach(buttonTwo);
  debouncer2.interval(debounceTime);

  matrix.begin(0x70);
  matrix.println(0);
  matrix.writeDigitNum(1, 0);
  matrix.writeDigitRaw(2, 0x04);
  matrix.writeDisplay();

  Serial.begin(9600);
  Serial.println("Initiated...");
}

void loop() {
  debouncer1.update();
  debouncer2.update();

  old_state = state;
  state = (debouncer1.read() << 1) | debouncer2.read();

  if (old_state == 0 && state != 3) {
    state = 0;
  }

  if (state == 3 && old_state != 3) {
    switch (old_state) {
      case 0:
        // both pressed
        Serial.println("Switching Modes");
        mode = !mode;
        break;
      case 1:
        // buttonOne pressed
        counts[mode]++;
        if (counts[mode] > MAX) {
          Serial.println("Count too high... Rolling over");
          counts[mode] = MIN;
          count = MIN;
        }
        break;
      case 2:
        // buttonTwo pressed
        counts[mode]--;
        if (counts[mode] > MAX) {
          Serial.println("Count too high... ");
          counts[mode] = MAX;
          count = MAX;
        }
        break;
      case 3:
        // none pressed
        break;
    }
    Serial.print("Count 1");
    Serial.print(" = ");
    Serial.println(counts[0]);
    Serial.print("Count 2");
    Serial.print(" = ");
    Serial.println(counts[1]);
    Serial.println();

    displayCount();
  }
}

void displayCount() {
  //  Serial.println(counts[0]%10);
  //  Serial.println(counts[0]/10);
  matrix.print(0);
  if (mode) {
    matrix.writeDigitRaw(2, 0x08);
  }
  else {
    matrix.writeDigitRaw(2, 0x04);

  }
  byte tens = counts[0] / 10;
  byte ones = counts[0] % 10;
  if (tens != 0) {
    matrix.writeDigitNum(3, tens);
  }
  matrix.writeDigitNum(4, ones);
  tens = counts[1] / 10;
  ones = counts[1] % 10;
  if (tens != 0) {
    matrix.writeDigitNum(0, tens);
  }
  matrix.writeDigitNum(1, ones);

  //  int countDisplay = counts[1] * 100 + counts[0];
  //  Serial.print("countDisplay = ");
  //  Serial.println(countDisplay);
  //  matrix.print(countDisplay);
  matrix.writeDisplay();
}

