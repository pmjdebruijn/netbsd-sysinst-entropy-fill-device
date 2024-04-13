// MIT License
// 
// Copyright (c) 2024 Pascal de Bruijn
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <Keyboard.h>

// encode 6 bits while avoiding sysinst shortcut keys, and shell relevants like $*?&!
const char rawEntropyEncoding[] = "()[]{}GHIJKLMNOPQRSTUVW:YZ"
                                  "@#%^.,ghijklmnopqrstuvw;yz"
                                  "0123456789+-";

const int analogNoisePin = A0; // unconnected
const int analogNoiseDelay = 25; // microseconds

byte getRawData() {
  delayMicroseconds(analogNoiseDelay);
  return bitRead(analogRead(analogNoisePin), 0);
}

bool getDebiasedRawData() {
  while (true)
  {
    byte firstBit = getRawData();
    byte secondBit = getRawData();

    if (firstBit != secondBit) { return firstBit; }
  }
}

byte getRawEntropy() {
  byte rawEntropy = 0;
  
  for (byte i = 0; i < 6; i++) {
    rawEntropy = (rawEntropy << 1) | getDebiasedRawData();
  }

  return rawEntropy;
}

byte repetitionValue = 255;
byte repetitionCount = 1;

int adaptiveProportionCount = 0;
int adaptiveProportionTable[64] = { 0 };

byte getTestedRawEntropy() {
  byte rawEntropy = getRawEntropy();

  // NIST SP 800-90B (4.4.1) Repetition Count Test
  if (rawEntropy == repetitionValue) {
    repetitionCount++;
    if (repetitionCount >= 4) {
      while (true) {} // block
    }
  }
  else {
    repetitionValue = rawEntropy;
    repetitionCount = 1;
  }

  // NIST SP 800-90B (4.4.2) Adaptive Proportion Test (without sliding window)
  adaptiveProportionCount++;
  adaptiveProportionTable[rawEntropy]++;
  for (byte i = 0; i < 64; i++) {
    if (adaptiveProportionTable[i] >= adaptiveProportionCount / 24 + 8) {
      while (true) {} // block
    }
  }

  return rawEntropy;
}

char getTestedRawEncodedEntropy() {
  return rawEntropyEncoding[getTestedRawEntropy()];
}

void setup() {
  Keyboard.begin(KeyboardLayout_en_US);

  while (millis() < 2000) {
    getTestedRawEncodedEntropy();
  }

  delay(500);

  Keyboard.println('a'); // a: Manually input random characters
  
  delay(500);

  for (int i = 0; i < 256; i++) {
    Keyboard.print(getTestedRawEncodedEntropy());
  }

  delay(500);

  Keyboard.println();
 
  delay(500);
  
  Keyboard.end();
}

void loop() {
}
