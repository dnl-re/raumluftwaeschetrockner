#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"

const char* ssid = "Transzendenz";
const char* password = "Mysteries.Lead-To#Relationship37!";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  initializeSerial();
  initializeOnboardLED();
  initializeRemoteLED();
  initializeWifi();
  initializeMqtt();
  initializeProgramConfiguration();
}

unsigned long previousMillis = 0;
boolean turningBarrelIsActive = false;
unsigned long currentMillis;
boolean programIsActive = false;

int barrelPin = 5;

int remoteLedPin = 0;
int remoteLedBrightness = 20;


struct {
  String operationType;
  long lengthOfPauseInMs;
  double degreesTurningBarrel;
  int lengthOfProgramInMs;
} programConfiguration, operationConfiguration;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Todo: ensure that json is parsed into long, double and int accordingly
  // Todo: integrate programConfiguration.lengthOfProgramInMs

  if (programIsActive) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= programConfiguration.lengthOfPauseInMs || turningBarrelIsActive) {
      previousMillis = currentMillis;
      turnBarrelByDegrees(programConfiguration.degreesTurningBarrel);
    }
  }
}

void setTurningBarrelActivity(boolean isActive) {
  turningBarrelIsActive = isActive;
}

long getInterval() {
  return programConfiguration.lengthOfPauseInMs;
}

long getCurrentMillis() {
  return currentMillis;
}

boolean getTurningBarrelActivity() {
  return turningBarrelIsActive;
}

void initializeSerial() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP gestartet");
}

void initializeOnboardLED() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void initializeRemoteLED() {
  pinMode(0, OUTPUT);
  analogWrite(remoteLedPin, LOW);
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

void initializeProgramConfiguration() {
  programConfiguration.operationType = "";
  programConfiguration.lengthOfPauseInMs = 50000;
  programConfiguration.lengthOfProgramInMs = 7 * 60 * 60 * 1000;
  programConfiguration.degreesTurningBarrel = 180.0;
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
      operationConfiguration.operationType = String(doc["operationType"]);

      handleOperationTypeStartProgram(doc);
      handleOperationTypeStopProgram(doc);
      handleOperationTypeStartSpinningBarrel(doc);
      handleOperationTypeStopSpinningBarrel(doc);

    } else {

      if (strPayload == "false") {
        deactivateTurningBarrel();
        Serial.println("Deactivated turning barrel");
      }
      if (strPayload == "true") {
        activateTurningBarrel();
        Serial.println("Activated turning barrel");
      }
    }
  }
}

void handleOperationTypeStartProgram(StaticJsonDocument<300> doc) {
  if (operationConfiguration.operationType == "startProgram") {
    programConfiguration.lengthOfPauseInMs = doc["lengthOfPauseInMs"];
    programConfiguration.lengthOfProgramInMs = doc["lengthOfProgramInMs"];
    programConfiguration.degreesTurningBarrel = doc["degreesTurningBarrel"];
    programIsActive = true;
    Serial.println("Started program with configuration: ");
    Serial.println("  - lengthOfPauseInMs: " + String(programConfiguration.lengthOfPauseInMs));
    Serial.println("  - lengthOfProgramInMs: " + String(programConfiguration.lengthOfProgramInMs));
    Serial.println("  - degreesTurningBarrel: " + String(programConfiguration.degreesTurningBarrel));
  }
}

void handleOperationTypeStopProgram(StaticJsonDocument<300> doc) {
  if (operationConfiguration.operationType == "stopProgram") {
    programIsActive = false;
    Serial.println("Stopped program with configuration left as it was: ");
    Serial.println("  - lengthOfPauseInMs: " + String(programConfiguration.lengthOfPauseInMs));
    Serial.println("  - lengthOfProgramInMs: " + String(programConfiguration.lengthOfProgramInMs));
    Serial.println("  - degreesTurningBarrel: " + String(programConfiguration.degreesTurningBarrel));
  }
}

void handleOperationTypeStartSpinningBarrel(StaticJsonDocument<300> doc) {
  if (operationConfiguration.operationType == "startSpinningBarrel") {
    activateTurningBarrel();
    Serial.println("Activated turning barrel");
  }
}

void handleOperationTypeStopSpinningBarrel(StaticJsonDocument<300> doc) {
  if (operationConfiguration.operationType == "stopSpinningBarrel") {
    deactivateTurningBarrel();
    Serial.println("Deactivated turning barrel");
  }
}