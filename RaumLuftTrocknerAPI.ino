void turnDrumByDegrees(double degrees) {
  double roundsPerMinute = 3;
  double millisecondsToTurnBarrel = calculateMillisecondsToTurnBarrel(roundsPerMinute, degrees);
  turnDrumByMilliseconds(millisecondsToTurnBarrel);
}

unsigned long previousMillis2 = getInterval();

void turnDrumByMilliseconds(double milliseconds) {
  if (!getTurningDrumActivity()) {
    activateTurningBarrel();
    setTurningDrumActivity(true);
    Serial.println("Started turning barrel. Debug ms: " + String(millis()));
  }

  if (getCurrentMillis() - previousMillis2 >= milliseconds) {
    previousMillis2 = getCurrentMillis() + getInterval();
    deactivateTurningBarrel();
    setTurningDrumActivity(false);
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
}

void deactivateTurningBarrel() {
  digitalWrite(LED_BUILTIN, HIGH);
}