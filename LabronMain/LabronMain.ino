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

void PrintByte(byte obj){
 for(int i = 7; i>-1; --i){
    byte t = (obj & (1<<(i)))!=0;
    Serial.print(t);
 } 
 Serial.println();
}

int count = 0;
void loop() {
  byte routeInfo = decideRoute();  // routeInfo organized [F,R,B,L; forward, right, left, turnaround]
  //routeInfo = 1;
  if (routeInfo != 0) {
    // now have new information to update with
    //Serial.println(direction);
    byte prevX = currentX;
    byte prevY = currentY;
    PrintByte(routeInfo);
    updateDirection(routeInfo);

    unsigned int positionPacket = ((prevX<<4) | (prevY));
    // Posn Packet as [XXXX-YYYY] 
    Serial.print("D = "); Serial.println(direction);
    Serial.print("X = "); Serial.println(currentX);
    Serial.print("Y = "); Serial.println(currentY);
    positionPacket = (positionPacket<<8) | (maze[prevX][prevY]) ;
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
