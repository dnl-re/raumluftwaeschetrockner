// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP gestartet");

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  turnDrumByDegrees(180.0);
  waitForSeconds(30);
}

void waitForSeconds(int seconds) {
  delay(seconds * 1000);
}

