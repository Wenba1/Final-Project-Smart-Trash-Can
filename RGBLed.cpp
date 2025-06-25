// RGBLed.cpp
#include "RGBLed.h"
#include <Arduino.h>

RGBLed::RGBLed(int redPin, int greenPin, int bluePin)
  : redPin(redPin), greenPin(greenPin), bluePin(bluePin) {}

void RGBLed::begin() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setColor("off");
}

void RGBLed::setColor(String color) {
  if (color == "red") {
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 255);
  } else if (color == "green") {
    analogWrite(redPin, 255);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
  } else if (color == "blue") {
    analogWrite(redPin, 0);
    analogWrite(greenPin,255);
    analogWrite(bluePin, 0);
  } else {
    
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
  }
}
