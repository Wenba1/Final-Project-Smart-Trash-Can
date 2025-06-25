#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <ESP32Servo.h>

class ServoController {
public:
  ServoController(int lidPin);
  void begin();
  void openLid();
  void closeLid();

private:
  int lidPin;
  Servo lidServo; 
};

#endif
