#include "sensors.h"
#include <Arduino.h>

void setupWallDetection() {
  pinMode(RIGHTWALL_PIN, OUTPUT);
  pinMode(FORWARDWALL_PIN, OUTPUT);
}

void setupIR() {
  pinMode(IRFRONT_PIN, INPUT);
  pinMode(IRRIGHT_PIN, INPUT);
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
  int averageRight = averageDistanceIRReading(wallDetectedDelay, IRRIGHT_PIN, wallDetectedAverage);

  bool right = averageRight > wallThreshold;
  bool front = averageForward > wallThreshold;

  // returning encoded wall packet
  // encoding goes (F, R, B[ehind], L, nullx4 so for example if wall was in front then first bit is high all others are 0
  if (right && front) {
    Serial.println(F("Right and Front"));
    return ( FRONT | RIGHT );
  }
  else if (right && !front) {
    Serial.println(F("Right"));
    return RIGHT;
  }
  else if (!right and front) {
    Serial.println(F("Front"));
    return FRONT;
  }
  else {
        Serial.println(F("nothing"));

    return NOWALL;
  }
}
