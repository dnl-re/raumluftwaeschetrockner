void turnDrumByDegrees(double degrees) {
  double roundsPerMinute = 3;
  double millisecondsToTurnBarrel = calculateMillisecondsToTurnBarrel(roundsPerMinute, degrees);
  turnDrumByMilliseconds(millisecondsToTurnBarrel);
}

void turnDrumByMilliseconds(double milliseconds) {
  activateTurningBarrel();
  Serial.println("Started turning barrel.");
  Serial.println("Milliseconds since start: " + String(millis()));
  delay(milliseconds);
  deactivateTurningBarrel();
  Serial.println("Stopped turning barrel after " + String(milliseconds) + " ms.");
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