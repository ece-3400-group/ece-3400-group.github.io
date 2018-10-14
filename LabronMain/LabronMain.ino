#include "servo.h"
#include "helpers.h"
#include "sensors.h"
#include "FFTs.h"

byte fftResult;

void rightHandWallFollow() {
  int wallDetect = wallDetected();
  if (wallDetect == 0) turnRight();
  else if (wallDetect == 1) turnLeft(); 
  else goStraight();
}

void setup() {
  serialBegin(9600);
  setupLineSensors();
  //setupServos();
  setupIR();
  setupFFT();
  setupDebugFFT();
}

void loop() {
 // rightHandWallFollow();
  wallDetected();
 // Serial.println("===============================================");
  Serial.println(analogRead(IRPinFront));
    //Serial.println("===============================================");

  //fftResult = readFFT(ADC5_FFT);
  //debugFFT();
  delay(50);
  displayLedFFT(fftResult, audioPin, hatPin, decoyPin);
}
