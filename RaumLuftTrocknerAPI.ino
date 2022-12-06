void turnBarrelByDegrees(double degrees) {
  double roundsPerMinute = 3;
  double millisecondsToTurnBarrel = calculateMillisecondsToTurnBarrel(roundsPerMinute, degrees);
  turnBarrelByMilliseconds(millisecondsToTurnBarrel);
}

unsigned long previousMillis2 = getInterval();

void turnBarrelByMilliseconds(double milliseconds) {
  if (!getTurningBarrelActivity()) {
    activateTurningBarrel();
    setTurningBarrelActivity(true);
    Serial.println("Started turning barrel. Debug ms: " + String(millis()));
  }

  if (getCurrentMillis() - previousMillis2 >= milliseconds) {
    previousMillis2 = getCurrentMillis() + getInterval();
    deactivateTurningBarrel();
    setTurningBarrelActivity(false);
    Serial.println("Stopped turning barrel after " + String(milliseconds) + " ms. Debug ms: " + String(millis()));
  }
}

double calculateMillisecondsToTurnBarrel(double roundsPerMinute, double degrees) {
  double minuteInMilliseconds = 60 * 1000;
  double degreesPerMinute = (roundsPerMinute * 360);
  double degreesPerMilliseconds = degreesPerMinute / minuteInMilliseconds;
  double millisecondsToTurnBarrel = degrees / degreesPerMilliseconds;
  return millisecondsToTurnBarrel;
}

void activateTurningBarrel() {
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(barrelPin, HIGH);
  analogWrite(remoteLedPin, remoteLedBrightness);
}

void deactivateTurningBarrel() {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(barrelPin, LOW);
  analogWrite(remoteLedPin, LOW);
}