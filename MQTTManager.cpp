#include "MQTTManager.h"
#include <ArduinoJson.h>

MQTTManager::MQTTManager(PubSubClient& client, ServoController* servo, RGBLed* led, UltrasonicSensor* ultrasonic, const char* topic,
                         bool& motionDetected, float& distanceCm, bool& lidOpen, String& ledColor, bool& automaticMode, String& fillingState)
  : mqttClient(client), servo(servo), rgbLed(led), ultrasonic(ultrasonic), updateTopic(topic),
    motionDetected(motionDetected), lastDistance(distanceCm), lidOpen(lidOpen),
    ledColor(ledColor), automaticMode(automaticMode), fillingState(fillingState) {}

void MQTTManager::setCallback() {
  mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
    handleShadowUpdate((const char*)payload);
  });
}

void MQTTManager::reconnectMQTT(const char* clientId) {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(clientId)) {
      Serial.println("connected");
      mqttClient.subscribe("$aws/things/smart_trash_can_sn0001/shadow/update/delta");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void MQTTManager::publishState() {
  StaticJsonDocument<512> doc;
  doc["state"]["reported"]["lid_open"] = lidOpen;
  doc["state"]["reported"]["depth_cm"] = lastDistance;
  doc["state"]["reported"]["filling_state"] = fillingState;
  doc["state"]["reported"]["led_color"] = ledColor;
  doc["state"]["reported"]["automatic_mode"] = automaticMode;
  doc["state"]["reported"]["motion_detected"] = motionDetected;

  char buffer[512];
  serializeJson(doc, buffer);

  Serial.println("===== AWS IoT Shadow Report =====");
  Serial.print("Topic: ");
  Serial.println(updateTopic);

  Serial.print("Payload: ");
  Serial.println(buffer);
  Serial.println("=================================");

  mqttClient.publish(updateTopic, buffer);
}

void MQTTManager::handleShadowUpdate(const char* payload) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print("JSON Error: ");
    Serial.println(error.c_str());
    return;
  }

  JsonObject desired = doc["state"];
  bool stateChanged = false;

   if (desired.containsKey("automatic_mode")) {
    bool mode = desired["automatic_mode"];
    if (mode != automaticMode) {
    automaticMode = mode;
    stateChanged = true;
    }
  }

  if (desired.containsKey("lid_open")) {
    bool open = desired["lid_open"];
    if (open != lidOpen) {
      if (open) {
        servo->begin();
        servo->openLid();
        lidOpen = true;
      } else {
        servo->begin();
        servo->closeLid();
        lidOpen = false;
      }
      stateChanged = true;
    }
  }


  if (desired.containsKey("led_color")) {
    const char* color = desired["led_color"];
    if (ledColor != String(color)) {
      ledColor = String(color);
      rgbLed->setColor(ledColor);
      stateChanged = true;
    }
  }

  if (desired.containsKey("filling_state")) {
    const char* stateValue = desired["filling_state"];
    if (fillingState != String(stateValue)) {
      fillingState = String(stateValue);
      stateChanged = true;
    }
  }

  if (stateChanged) {
    publishState();
  }
}
