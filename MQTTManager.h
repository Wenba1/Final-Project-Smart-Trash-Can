#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include "ServoController.h"
#include "RGBLed.h"
#include "UltrasonicSensor.h"

class MQTTManager {
  public:
    MQTTManager(PubSubClient& client, ServoController* servo, RGBLed* led, UltrasonicSensor* ultrasonic, const char* topic,
                bool& motion, float& distance, bool& door, String& ledColor, bool& autoMode, String& fillingState);

    void setCallback();
    void reconnectMQTT(const char* clientId);
    void publishState();

  private:
    PubSubClient& mqttClient;
    ServoController* servo;
    RGBLed* rgbLed;
    UltrasonicSensor* ultrasonic;
    const char* updateTopic;

    bool& motionDetected;
    float& lastDistance;
    bool& lidOpen;
    String& ledColor;
    bool& automaticMode;
    String& fillingState;

    void handleShadowUpdate(const char* payload);
};

#endif

