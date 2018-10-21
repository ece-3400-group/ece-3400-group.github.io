#include "servo.h"
#include "helpers.h"
#include "sensors.h"
#include "FFTs.h"
# include "radios.h"
byte fftResult;

void setup() {
  serialBegin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  setupLineSensors();
  setupServos();
  setupIR();
  setupFFT();
  setupDebugFFT();
  setupRadios();
  setupWallDetection();
}
int count = 0;
void loop() {
 // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //delay(500);
  //wallDetected();
  byte routeInfo = decideRoute();  // routeInfo organized [F,R,B,L; forward, right, left, turnaround]
  if (routeInfo != 0){
     if (bitRead(routeInfo, 3)){
        //we're moving forward - coordinate this with current position and direction for updates
     }
     else if(bitRead(routeInfo, 2)){
        //we're turning right - coordinate this with current position and direction for updates
     }
     else if(bitRead(routeInfo, 1)){
        //we're turning left - coordinate this with current position and direction for updates
     }
     else if(bitRead(routeInfo, 0)){
        //we're turning around - we'll need to adjust so that we somehow know whenever turnaround is called where our position is (maybe put all this in its own file)
     }

     if (bitRead(routeInfo, 7)) {
        // there's a forward wall}
     }
     if (bitRead(routeInfo, 6)) {
        // there's a right wall}
     }
     if (bitRead(routeInfo, 5)) {
        // there's a left wall}
     }
     

    //heres where we'll transmit to radio
  }
  if (count == 0){
    fftResult = readFFT(ADC5_FFT);
    debugFFT();
  }
  displayLedFFT(fftResult, audioPin, hatPin, decoyPin);
  count++;
  count = count % 3;
}
