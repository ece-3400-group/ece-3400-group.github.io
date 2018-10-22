#include "servo.h"
#include "helpers.h"
#include "sensors.h"
#include "FFTs.h"
# include "radios.h"
byte fftResult;

void setup() {
  serialBegin(9600);
  setupLineSensors();
  setupServos();
  setupIR();
  setupFFT();
  setupDebugFFT();
  setupRadios();
  setupWallDetection();
  while( waitForStart() == 1 );
  Serial.println("STARTING");
}

int count = 0;
void loop() {

  decideRoute();
  if (count == 0){
    debugFFT();
    fftResult = readFFT(ADC5_FFT);
    while (fftResult & IRHAT_MASK) {
      fftResult = readFFT(ADC5_FFT);
      stop();
    }
  }
  displayLedFFT(fftResult, AUDIO_PIN, IRHAT_PIN );
  count++;
  count = count % 3;
  packetTransmission(0x01, 0x02);
}
