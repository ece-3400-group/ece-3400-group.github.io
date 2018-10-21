#include "sensors.h"
#include <Arduino.h>

const int IRPinFront = A3; 
const int IRPinRight = A4;
const int RightWallPin = 11;
const int ForwardWallPin = 12;

const int wallThreshold = 300;


void setupWallDetection() {
  pinMode(RightWallPin, OUTPUT);
  pinMode(ForwardWallPin, OUTPUT);
}

void setupIR() {
  pinMode(IRPinFront, INPUT);
  pinMode(IRPinRight, INPUT);
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
  int delayTime = 30; 
  const int n = 5;

  int averageForward = averageDistanceIRReading(delayTime, IRPinFront, n); // take an average to be less sensitive to noise
  int averageRight = averageDistanceIRReading(delayTime, IRPinRight, n);

  bool right = true ? averageRight > wallThreshold : false;
  bool front = true ? averageForward > wallThreshold : false;

  // returning encoded wall packet
  // encoding goes (F, R, B[ehind], L, nullx4 so for example if wall was in front then first bit is high all others are 0
  if (right && front) return 0b11000000;
  else if (right && !front) return 0b01000000;
  else if (!right and front) return 0b10000000;
  else return 0b00000000;

  
}
