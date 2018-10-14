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
    //Serial.print("Readings " ); Serial.println(readings[i]);
    average += readings[i];
  }
  average = average/(n); // avoid integer division issues
//  Serial.print("Average "); 
//  Serial.println(average);
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
   // Serial.print("R sensor doesn't detect wall = ");
   // Serial.println(averageRight);
    return 0;     // Turn right (R sensor doesn't detect wall)
  }
  else{
    if(averageForward > wallThreshold) {
      Serial.print("FORWARD WALL DETECTED = "); Serial.println(averageForward);
      return 1;   // Turn left (both F and R sensors detect wall)
    }
  //  Serial.print("Wall detected! = "); Serial.println(averageRight);
    return 2;     // Continue moving forward (only R sensor detects wall)
  }
}
