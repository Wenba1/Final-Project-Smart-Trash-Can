#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Arduino.h>

class MotionSensor {
private:
    int pin;
    bool lastState;

public:
    MotionSensor(int pin);  
    void begin();           
    //bool detectMotionForDuration(unsigned long durationMs );   
    bool detectMotionOverTime(unsigned long durationMs = 1500); 
};

#endif
