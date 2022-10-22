#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"

const char* ssid = "Transzendenz";
const char* password = "*********************";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);


// Todo: Either call it barrel or drum. Not both

void setup() {
  initializeSerial();
  initializeLEDs();
  initializeWifi();
  initializeMqtt();
}

unsigned long previousMillis = 0;
boolean turningDrumIsActive = false;
const long interval = 15000;
unsigned long currentMillis;


struct {
  long lengthOfPauseInMs;
  int degreesTurningBarrel;
  int lengthOfProgramInMs;
} operationConfiguration;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  currentMillis = millis();
  if (currentMillis - previousMillis >= interval || turningDrumIsActive) {
    previousMillis = currentMillis;
    turnDrumByDegrees(180.0);
  }
}

void setTurningDrumActivity(boolean isActive) {
  turningDrumIsActive = isActive;
}

long getInterval() {
  return interval;
}

long getCurrentMillis() {
  return currentMillis;
}

boolean getTurningDrumActivity() {
  return turningDrumIsActive;
}

void initializeSerial() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP gestartet");
}

void initializeLEDs() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void initializeWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Start connecting to wifi '" + String(ssid) + "'.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Retry connecting to wifi '" + String(ssid) + "'.");
  }
  Serial.println("Successfully connected to wifi '" + String(ssid) + "'.");
}

void initializeMqtt() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void waitForSeconds(int seconds) {
  int milliseconds = seconds * 1000;
  Serial.println("Waiting for " + String(milliseconds) + " ms.");
  delay(milliseconds);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266-LedClient")) {
      client.subscribe("Raumlufttrockner/Led");
      Serial.println("Successfully subscribed to 'Raumlufttrockner/Led' topic.");
    } else {
      delay(6000);
      Serial.println("Retrying to subscribe to 'Raumlufttrockner/Led' topic.");
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String strTopic = String(topic);
  String strPayload = String((char*)payload);
  Serial.println("Received message from topic " + strTopic + " : " + strPayload);

  if (strTopic == "Raumlufttrockner/Led") {
    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, strPayload);


    if (!error) {

      Serial.println("parsing json: ");

      long lengthOfPauseInMs = doc["lengthOfPauseInMs"];

      operationConfiguration.lengthOfPauseInMs = doc["lengthOfPauseInMs"];
      operationConfiguration.lengthOfProgramInMs = doc["lengthOfProgramInMs"];
      operationConfiguration.degreesTurningBarrel = doc["degreesTurningBarrel"];


    } else {

      if (strPayload == "false") {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      if (strPayload == "true") {
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
  }
}
