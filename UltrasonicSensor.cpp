#include "UltrasonicSensor.h"
#include <Arduino.h>

UltrasonicSensor::UltrasonicSensor(int trigPin, int echoPin)
  : trigPin(trigPin), echoPin(echoPin) {}

void UltrasonicSensor::begin() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float UltrasonicSensor::getDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) {
    return -1.0; 
  }

  float distance = duration * 0.034 / 2; 

  if (distance < 0.0 || distance > 15.0) {
    return -1.0;
  }

  return distance;
}

