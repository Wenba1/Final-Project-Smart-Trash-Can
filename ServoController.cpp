#include "ServoController.h"

ServoController::ServoController(int lidPin)
  : lidPin(lidPin) {}

void ServoController::begin() {
  lidServo.attach(lidPin);
  closeLid();
}

void ServoController::openLid() {
  lidServo.write(90);
}

void ServoController::closeLid() {
  lidServo.write(0);
}
