// RGBLed.h
#ifndef RGB_LED_H
#define RGB_LED_H

#include <Arduino.h>

class RGBLed {
public:
  RGBLed(int redPin, int greenPin, int bluePin); 
  //RGBLed(int greenPin, int bluePin, int redPin, ); 
  
  void begin();
  void setColor(String color); 

private:
  int redPin;
  int greenPin;
  int bluePin;
};

#endif
