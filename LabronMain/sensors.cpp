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
  bool front = averageForward > wallThreshold;

  int averageRight = averageDistanceIRReading(wallDetectedDelay, IRRIGHT_PIN, wallDetectedAverage);
  bool right = averageRight > wallThreshold;

  /*
   * digitalWrite(MUX_PIN, HIGH);  // Now we've switched to checking the left wall
   * 
   */

//  int averageLeft = averageDistanceIRReading(wallDetectedDelay, IRRIGHT_PIN, wallDetectedAverage);
//  bool left = averageLeft > wallThreshold;

  /*
   * digitalWrite(MUX_PIN, LOW);  // Now we've gone back to checking the right wall for future use
   */

  // returning encoded wall packet
  // encoding goes (F, R, B[ehind], L, nullx4 so for example if wall was in front then first bit is high all others are 0
  byte directionPacket = 0b00000000;
  if (right) {
    Serial.println(F("Right"));
    directionPacket |= RIGHT;
  }
  if (front) {
    Serial.println(F("Front"));
    directionPacket |= FRONT;
  }
//  if (left) {
//    Serial.println(F("LEFT"));
//    directionPacket |= LEFT;
//  }
  else {
    Serial.println(F("nothing"));
    directionPacket |= NOWALL;
  }
  return directionPacket;
}
