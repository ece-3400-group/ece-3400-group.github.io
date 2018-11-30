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
 // setupServos();
  setupIR();
  setupFFT();
  setupDebugFFT();
  setupRadios();
  // Starting location is the root of everything
  byte firstByte = byteifyCoordinate(currentX,currentY); // Initial starting point stack push 
  dfsStackPush(firstByte);
  pathStackPush(firstByte); 
  while( waitForStart() );
}

int count = 0;

void loop() {
  Serial.println("Started");
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
    Serial.println(F("COMPLETE STOP"));
    stop();
  }
  if (count == 0){
   // debugFFT();
    while (fftResult & readFFT(ADC5_FFT) & IRHAT_MASK) {
      Serial.println("IR HAT DETECTED");
      fftResult = 0;
      stop();
    }
    fftResult = readFFT(ADC5_FFT);
  }
  //displayLedFFT(fftResult, AUDIO_PIN, IRHAT_PIN );
  count++;
  count = count % 3;
}
