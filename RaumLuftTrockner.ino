#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Transzendenz";
const char* password = "*******************";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP gestartet");

  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Start connecting to wifi '" + String(ssid) + "'.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Retry connecting to wifi '" + String(ssid) + "'.");
  }
  Serial.println("Successfully connected to wifi '" + String(ssid) + "'.");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

unsigned long previousMillis = 0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  const long interval = 30000;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    turnDrumByDegrees(180.0);
  }
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
    if (strPayload == "false") {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if (strPayload == "true") {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}