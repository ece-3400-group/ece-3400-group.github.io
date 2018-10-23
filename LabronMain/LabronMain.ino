#include "maze.h"
#include "ffts.h"
#include "servo.h"
#include "radios.h"
#include "sensors.h"
byte fftResult;

void setup() {
  Serial.begin(57600); // radios want this
  setupLineSensors();
  populateMazeStart();
  setupServos();
  setupIR();
  setupFFT();
  setupDebugFFT();
  setupRadios();
  while( waitForStart() );
}

int count = 0;
void loop() {
  byte routeInfo = decideRoute();  // routeInfo organized [F,R,B,L; forward, right, left, turnaround]
  if (routeInfo != 0) {
    Serial.println(currentX);
    Serial.println(currentY);
    // now have new information to update with
    unsigned int positionPacket = ((currentX<<4) | (currentY));
    positionPacket = (positionPacket<<8) | (updateDirection(routeInfo));
<<<<<<< HEAD
    packetTransmission(positionPacket);
=======
    while (packetTransmission(positionPacket) == 0) packetTransmission(positionPacket);
    //delay(300);
>>>>>>> 4b2e87ba74ae02d0682157a01662d9aa574a5c0c
    Serial.println("=======================================");
     Serial.println(positionPacket);
   Serial.println("=======================================");

  }
  if (count == 0){
   // debugFFT();
    fftResult = readFFT(ADC5_FFT);
    while (fftResult & IRHAT_MASK) {
      fftResult = readFFT(ADC5_FFT);
      stop();
    }
  }
  //displayLedFFT(fftResult, AUDIO_PIN, IRHAT_PIN );
  count++;
  count = count % 3;
}
