#include "maze.h"
#include "servo.h"
#include "helpers.h"
#include "sensors.h"
#include "FFTs.h"
#include "radios.h"
byte fftResult

void setup() {
  Serial.begin(57600); // radios want this
  setupLineSensors();
  populateMazeStart();
  setupServos();
  setupIR();
  setupFFT();
  setupDebugFFT();
  setupRadios();
  while( waitForStart() == 1 );
}

int count = 0;
void loop() {
  byte routeInfo = decideRoute();  // routeInfo organized [F,R,B,L; forward, right, left, turnaround]
  if (routeInfo != 0x00) {
    // now have new information to update with
    unsigned int positionPacket = ((currentX<<4) | (currentY));
    positionPacket = (positionPacket<<8) | (updateDirection(routeInfo));
    packetTransmission(positionPacket);
    Serial.print("TX - " ); Serial.println(positionPacket);
  }
  if (count == 0){
    debugFFT();
    fftResult = readFFT(ADC5_FFT);
    while (fftResult & IRHAT_MASK) {
      fftResult = readFFT(ADC5_FFT);
      Serial.println("ir");
      stop();
    }
  }
  //displayLedFFT(fftResult, AUDIO_PIN, IRHAT_PIN );
  count++;
  count = count % 3;
}
