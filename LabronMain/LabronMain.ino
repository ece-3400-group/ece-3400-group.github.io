#include "maze.h"
#include "ffts.h"
#include "servo.h"
#include "radios.h"
#include "sensors.h"
#include "helpers.h"

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
  byte routeInfo = decideRouteDFS();  // routeInfo organized [F,R,B,L; forward, right, left, turnaround]
  //routeInfo = 1;
  if (routeInfo != 0) {
    // now have new information to update with
    //Serial.println(direction);
    //Serial.println(routeInfo);
    updateDirection(routeInfo);
 
    unsigned int positionPacket = byteifyCoordinate(currentX,currentY);
    // Position Packet as [XXXX-YYYY] 
    Serial.print(F("D = ")); Serial.println(direction);
    Serial.print("X = "); Serial.println(currentX);
    Serial.print("Y = "); Serial.println(currentY);
    for (int i = 0; i < 9; i++ ) {
      for (int j = 0; j < 9; j++ ){ //SENW
          Serial.print((maze[i][j], BIN)); Serial.print(F(" "));
      }
      Serial.println(F(""));
    }
    positionPacket = (positionPacket<<8) | ((maze[currentX][currentY]) & 0x00FF);
    while (packetTransmission(positionPacket) == 0) {
      packetTransmission(positionPacket);
      //delay(300);
    }
    //delay(300);
    Serial.println(F("(======================================="));
     Serial.println(positionPacket);
   Serial.println(F("======================================="));

  }
  while ((routeInfo & FRONTWALL) && (routeInfo & RIGHTWALL) && (routeInfo & LEFTWALL) || checkMazeEmpty()) { // checkMazeEmpty returns true if all nodes explored
    Serial.println("COMPLETE STOP");
    stop();
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
