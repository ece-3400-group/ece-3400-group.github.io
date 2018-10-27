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
  //routeInfo = 1;
  if (routeInfo != 0) {
    // now have new information to update with
    //Serial.println(direction);
    //Serial.println(routeInfo);
    updateDirection(routeInfo);
 
    unsigned int positionPacket = ((currentX<<4) | (currentY)) & (0x00FF);
    // Posn Packet as [XXXX-YYYY] 
    Serial.print("D = "); Serial.println(direction);
    Serial.print("X = "); Serial.println(currentX);
    Serial.print("Y = "); Serial.println(currentY);
    for (int i = 0; i < 9; i++ ) {
      for (int j = 0; j < 9; j++ ){ //SENW
          Serial.print(maze[i][j], BIN); Serial.print(" ");
      }
      Serial.println("");
    }
    positionPacket = (positionPacket<<8) | ((maze[currentX][currentY]) & 0x00FF);
    while (packetTransmission(positionPacket) == 0) {
      packetTransmission(positionPacket);
      //delay(300);
    }
    //delay(300);
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
