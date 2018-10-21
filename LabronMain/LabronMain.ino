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
  while( waitForStart() == 0b0001);
}
int count = 0;
void loop() {
  decideRoute();
  if (count == 0){
    debugFFT();
              fftResult = readFFT(ADC5_FFT);

    while (fftResult & IRHAT_MASK) {
          fftResult = readFFT(ADC5_FFT);
       Serial.println(" THE IRHAT");
      //Serial.println("TURNING AROUND");
      //turnAround();
      stop();
    }
  }
  displayLedFFT(fftResult, audioPin, hatPin, decoyPin);
  count++;
  count = count % 2;
}
