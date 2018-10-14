#include "sensors.h"
#include <Arduino.h>

const int IRPinFront = A3; 
const int IRPinRight = A4;
const int wallThreshold = 230;

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

int wallDetected(){
  // Using an average measurement for a period of timesteps
  // decide if a wall is there.
  int delayTime = 30; 
  const int n = 5;

  int averageForward = averageDistanceIRReading(delayTime, IRPinFront, n); // take an average to be less sensitive to noise
  int averageRight = averageDistanceIRReading(delayTime, IRPinRight, n);

  if (averageRight < wallThreshold){
    return 0;     // Turn right (R sensor doesn't detect wall)
  }
  else{
    if(averageForward > wallThreshold) {
      return 1;   // Turn left (both F and R sensors detect wall)
    }
    return 2;     // Continue moving forward (only R sensor detects wall)
  }
}
