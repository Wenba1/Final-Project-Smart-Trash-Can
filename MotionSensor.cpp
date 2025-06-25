#include "MotionSensor.h"

MotionSensor::MotionSensor(int inputPin) : pin(inputPin), lastState(false) {}

void MotionSensor::begin() {
    pinMode(pin, INPUT);
}

//bool MotionSensor::detectMotionForDuration(unsigned long durationMs) {
  //unsigned long start = millis();
 // while (millis() - start < durationMs) {
   // if (digitalRead(pin) == HIGH) {
   //   lastState = true;
  //    return true;
  // }
 // }
 // lastState = false;
 // return false;
//}

bool MotionSensor::detectMotionOverTime(unsigned long durationMs) {
    unsigned long startTime = millis();
    while (millis() - startTime < durationMs) {
        if (digitalRead(pin) == HIGH) {
            return true;
        }
    }
    return false;
}
