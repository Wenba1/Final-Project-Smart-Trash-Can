#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

class UltrasonicSensor {
public:
  UltrasonicSensor(int trigPin, int echoPin);
  void begin();
  float getDistanceCm();

private:
  int trigPin;
  int echoPin;
};

#endif