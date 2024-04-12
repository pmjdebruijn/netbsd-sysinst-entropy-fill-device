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

byte getAnalogNoise() {
  delayMicroseconds(analogNoiseDelay);
  return bitRead(analogRead(analogNoisePin), 0);
}

bool getDebiasedAnalogNoise() {
  while (true)
  {
    byte firstBit = getAnalogNoise();
    byte secondBit = getAnalogNoise();

    if (firstBit != secondBit) { return firstBit; }
  }
}

byte getRawEntropy() {
  byte rawEntropy = 0;
  
  for (byte i = 0; i < 6; i++) {
    rawEntropy = (rawEntropy << 1) | getDebiasedAnalogNoise();
  }

  return rawEntropy;
}

char getRawEncodedEntropy() {
  return rawEntropyEncoding[getRawEntropy()];
}

void setup() {
  Keyboard.begin();

  delay(3000);

  Keyboard.println('a'); // a: Manually input random characters
  
  delay(500);

  for (int i = 0; i < 256; i++) {
    Keyboard.print(getRawEncodedEntropy());
  }

  delay(500);

  Keyboard.println();
 
  delay(500);
  
  Keyboard.end();
}

void loop() {
}
