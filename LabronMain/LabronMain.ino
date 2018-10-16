#include "servo.h"
#include "helpers.h"
#include "sensors.h"
#include "FFTs.h"

byte fftResult;

void setup() {
  serialBegin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  setupLineSensors();
  setupServos();
  setupIR();
  setupFFT();
  setupDebugFFT();
  setupWallDetection();
}
int count = 0;
void loop() {
 // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //delay(500);
  //wallDetected();
  decideRoute();
  if (count == 0){
    fftResult = readFFT(ADC5_FFT);
    debugFFT();
  }
  displayLedFFT(fftResult, audioPin, hatPin, decoyPin);
  count++;
  count = count % 3;
}
