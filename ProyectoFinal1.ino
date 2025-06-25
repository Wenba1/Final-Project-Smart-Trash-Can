#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "ServoController.h"
#include "RGBLed.h"
#include "MotionSensor.h"
#include "UltrasonicSensor.h"
#include "MQTTManager.h"

const int PIR_PIN = 19;
const int SERVO_PIN = 18;
const int RGB_R_PIN = 21;
const int RGB_G_PIN = 23;
const int RGB_B_PIN = 22;
const int TRIGGER_PIN = 2;
const int ECHO_PIN = 4;

const char AMAZON_PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAoMU4SibPpItYWXiEqm8naajOBnGfbz7t7xWwrHRTIxTY9AzB
W+0HfLz7geQP4dC6KaUixRC1tYI8pbRXRCw+m/UDPhs5DqMVKcELTCcqaAVHA8Ci
O92RKo+isO0Utze9WMGDJyNAlx3MY4PFld+UX33h2KwRjaYpS8SWGcBLG5Jr9r2v
n/qY381EZQMzKGclDuK0sJiKBDG2H5avPFsDBqHSDWyfGoJyLcyJk4xhr4IpDKAQ
scBkcOC+W5kFFLXVRoDWOEZjEwVvwrNnjWmQtKhEkJoq7gSFYVzLUq/VxqqQdTWk
KGD5qQ9WaWxSz/TK2Z7JSz3zIvVAjxPIHrkKvwIDAQABAoIBAA/zQSBqNVOX144Y
3OCj4n8jFj2hbDX3QlfkCQEzEUR7vwKHeW+LVqDThwhd/g1jTfTvw0FV5RE6+JVg
DQBge12xsz6oKMMA/wqDcoF9qUFktKT0qaPS2aIlWfi/rLx4tTpnCIILh072z95X
pnS2F9xZetyqRZuODq7LtDCd2VXEytaO8el4fTbNnqUxThAx7LB3e5IM/nTnHvhV
W9iPN1SyyyQnN+fTZ8qDifx+sZBIBbq8k/0sRk703Qlugu1aXGpDlxz3Xfa6IRHr
qXpjhQNXQ/oLwfOp2j57qTaoNIZqYkJu0UHAQaS7kwWHfP+QOY+LHWRZkiZN4N09
isEzaRECgYEA0ZhiAs18TnPtUJuvi6CR0muPBh7YoYf1GCN9iY34obf67tPJc++D
+u1t/BgbB0zNK16NWbBgEt8KMVWM27atDJZ5K/IUE+A5R/Rdg7YE5MZapjloRXI9
FIsThB+6dR4bgxSIzMNUpDgn6QfzNnpjiXREpmU6/KQsZyxD4D7hyn0CgYEAxF2A
HZGXdk9qQA7wdwsaCEmNAb4Pk/74WafUAIRK6x1GHcYu3CVF0unvB7FvYdP1Z2eV
j3w1Q/ohCz2AdC4vdPUCpmTPNidgND7pe1hCcOq/qwuwXGlIHR4vJY/6r4lXCbkk
Y3WuutNdtkFTNqwjhPfetMFbc9y565xb1fhS8usCgYBUkWKCdPzB/t5zFs4mzVvp
BgMIWivYSS4oVjGR7dmzDrPN52AySiL7fQ9SlztYn7Fs3VK1LIgnNR08uKMEB9oc
6R3tEFjviBo0oTGWMjwZPrtWtHURCD4C6vat6ctm9AXvxrpHvEyLkijwSofkpE9D
J8mHRfMd+vP7kQ22Dkk2VQKBgQCkRPfNRKOphKwE5LqDS9U+Q5FjeMT9cNn/3qc6
u3EVGQs0EzvWTJQJ6ktdazEbbL6p/3ydaTICKPiFt7CDALQQqnrKCiFSU0Rm0I5v
8hi675biF0g1NpnuSfb0/GqheEpIxwpK7gL7JaEKu8FKRlz8eoNjQTIpTeh20vn8
mN2DewKBgDs1qRu73CYmrE2FXSLaujpSedy7rP2IIh/QECtcXEGzClDOHfrYZKTN
9omDvjrv2TqOBtvpBhFYJvjb+HdoZcYjGcCsxHHyr3/haSLWjnpl8O/XryiE6G7s
mCYG2tH3V5sIh2M8nlcayVyOQ+uJ+Ctu2zt47M1ivOe8TxHn0tBb
-----END RSA PRIVATE KEY-----
)KEY";

const char AMAZON_CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUQbk3w1gBz6m0028M8n1+1FMUsjkwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDUyNTE2NTcw
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKDFOEomz6SLWFl4hKpv
J2mozgZxn28+7e8VsKx0UyMU2PQMwVvtB3y8+4HkD+HQuimlIsUQtbWCPKW0V0Qs
Ppv1Az4bOQ6jFSnBC0wnKmgFRwPAojvdkSqPorDtFLc3vVjBgycjQJcdzGODxZXf
lF994disEY2mKUvElhnASxuSa/a9r5/6mN/NRGUDMyhnJQ7itLCYigQxth+Wrzxb
Awah0g1snxqCci3MiZOMYa+CKQygELHAZHDgvluZBRS11UaA1jhGYxMFb8KzZ41p
kLSoRJCaKu4EhWFcy1Kv1caqkHU1pChg+akPVmlsUs/0ytmeyUs98yL1QI8TyB65
Cr8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUQaa6sPcYUgO6LUZTNDtBZNd1aCowHQYD
VR0OBBYEFPR7mju5nmIlo4abnP9LwmIPbvgWMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCq0OsIQTRGCu+Vc28j8X0V2V8I
5VZrfhPm4WXBaOoSC0KW3qF/vKChZCbc/UVHzrNptqFDJnxYfdtWs+JBJHuKTWmG
KhhDBdR7nOmMb4X2chirxn+cT8ipDoWpTuHvWOayFmRsZ3PcyDK/0PxJXRca1olc
ZXhzMQEYM532FC4BdUGx7/LMfneWJXntzZxicQvQeQipZPZtqU6iW2AMTOojM78c
WS2tJ2cLmIoqtFurFs/fGX35Ip3FN434LROYaLMCLr86hztCOn2eUE2x6MySvQUb
JfveUjMkvwvjaFZM4IN21OOvkTsNhZMrrhskSwi+rTcHbBEdONIep4cXKwei
-----END CERTIFICATE-----
)KEY";

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";


// Estado global
bool motionDetected = false;
float distanceCm = 0.0;
bool lidOpen = false;
bool automaticMode = false;
String fillingState = "empty";
String ledColor = "off";

// Objetos hardware
ServoController servo(SERVO_PIN);

RGBLed rgbLed(RGB_G_PIN, RGB_B_PIN, RGB_R_PIN);
//RGBLed rgbLed(RGB_R_PIN, RGB_G_PIN, RGB_B_PIN);

MotionSensor pir(PIR_PIN);
UltrasonicSensor ultrasonic(TRIGGER_PIN, ECHO_PIN);

// WiFi + MQTT
WiFiClientSecure net;
PubSubClient mqttClient(net);
const char* updateTopic = "$aws/things/smart_trash_can_sn0001/shadow/update";
MQTTManager mqtt(mqttClient, &servo, &rgbLed, &ultrasonic, updateTopic,
                 motionDetected, distanceCm, lidOpen, ledColor, automaticMode, fillingState);


void setup() {
  Serial.begin(115200);
  servo.begin();
  rgbLed.begin();
  pir.begin();
  ultrasonic.begin();

  WiFiManager wm;
  wm.setConfigPortalTimeout(180);
  if (!wm.autoConnect("SmartTrashCan_ConfigAP")) {
    ESP.restart();
  }
  Serial.println("WiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());


  net.setCACert(AMAZON_ROOT_CA1);
  net.setCertificate(AMAZON_CERTIFICATE);
  net.setPrivateKey(AMAZON_PRIVATE_KEY);

  mqttClient.setServer("ahxlba0g2uxk8-ats.iot.us-east-1.amazonaws.com", 8883);
  mqtt.setCallback();
  //mqtt.begin();
 
}


void loop() {
  if (!mqttClient.connected()) {
    mqtt.reconnectMQTT("smart_trash_can");
  }
  mqttClient.loop();

  if (automaticMode && !lidOpen) {
    motionDetected = pir.detectMotionOverTime(1500);
    if (motionDetected) {
      mqtt.publishState();
    }
  }

  float newDistance = ultrasonic.getDistanceCm();
  if (newDistance != -1.0 && abs(newDistance - distanceCm) > 1.0) {
    distanceCm = newDistance;
    mqtt.publishState();
  }

  delay(5000);
}
