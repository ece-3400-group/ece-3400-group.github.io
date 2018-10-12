#include "servo.h"
#include "helpers.h"
#include "sensors.h"
#include "FFTs.h"
const int audioPin = 3;
const int hatPin = 4;
const int decoyPin = 5;

void rightHandWallFollow() {
  int wallDetect = wallDetected();
  if (wallDetect == 0) turnRight();
  else if (wallDetect == 1) turnLeft(); 
  goStraight();
}

void setup() {
  serialBegin(9600);
  setupLineSensors();
  setupServos();
  setupIR();
  setupFFT();
  pinMode(audioPin, OUTPUT);
  pinMode(hatPin, OUTPUT);
  pinMode(decoyPin, OUTPUT);
}

void loop() {
  rightHandWallFollow();
  byte fftResult = readFFT(ADC0_FFT);
  displayLedFFT(fftResult, audioPin, hatPin, decoyPin);
}
