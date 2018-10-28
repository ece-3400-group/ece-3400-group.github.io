#include "sensors.h"
#include <Arduino.h>

void setupWallDetection() {
  pinMode(RIGHTWALL_PIN, OUTPUT);
  pinMode(FORWARDWALL_PIN, OUTPUT);
}

void setupIR() {
  pinMode(IRFRONT_PIN, INPUT);
  pinMode(IRMUX_PIN, OUTPUT);
  pinMode(IRLEFTRIGHT_PIN, INPUT);
}

int averageDistanceIRReading(int delayTime, const int IRPin, int n){
  // Average n measurements from specified IRPin separated by delayTime amount of time
  int readings [n] = {};
  for (int i=0; i<n; i++){
    readings[i] = analogRead(IRPin);
    delay(delayTime);
  }
  int average = 0;
  for (int i=0; i<n; i++){
    average += readings[i];
  }
  average = average/(n); // avoid integer division issues
  return average;
}

byte wallDetected(){
  // Using an average measurement for a period of timesteps
  // decide if a wall is there.
  // return the correct byte describing the walls around the robot

  int averageForward = averageDistanceIRReading(wallDetectedDelay, IRFRONT_PIN, wallDetectedAverage); // take an average to be less sensitive to noise
  bool front = averageForward > wallThreshold;

  digitalWrite(IRMUX_PIN, IRRIGHT);  // Now we've switched to checking the left wall
  int averageRight = averageDistanceIRReading(wallDetectedDelay, IRLEFTRIGHT_PIN, wallDetectedAverage);
  bool right = averageRight > wallThreshold;

  digitalWrite(IRMUX_PIN, IRLEFT);  // Now we've gone back to checking the right wall for future use
  int averageLeft = averageDistanceIRReading(wallDetectedDelay, IRLEFTRIGHT_PIN, wallDetectedAverage);
  bool left = averageLeft > wallThreshold;

  // returning encoded wall packet
  // encoding goes (F, R, B[ehind], L, nullx4 so for example if wall was in front then first bit is high all others are 0
  byte directionPacket = 0b00000000;
  if (right) {
    Serial.println(F("RIGHT"));
    directionPacket |= RIGHT;
  }
  if (front) {
    Serial.println(F("FRONT"));
    directionPacket |= FRONT;
  }
 if (left) {
   Serial.println(F("LEFT"));
   directionPacket |= LEFT;
 }
  else {
    Serial.println(F("nothing"));
    directionPacket |= NOWALL;
  }
  return directionPacket;
}
