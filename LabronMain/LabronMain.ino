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
  populateMazeStart(); // all locs unexplored
  setupServos();
  setupIR();
  setupFFT();
  setupDebugFFT();
  setupRadios();
  // Starting location is the root of everything
  void clearStacks(); 
  byte firstByte = byteifyCoordinate(currentX,currentY); // Initial starting point stack push 
  dfsStackPush(firstByte);
  pathStackPush(firstByte); 
  //while( waitForStart() );
}

int count = 0;

void loop() {
  if (checkMazeEmpty()){
    Serial.println(F("MAZE IS EXPLORED LEBRON IS DONE")); 
  }
  byte routeInfo = decideRouteDFS(); 
  // routeInfo organized [F,R,B,L; forward, right, left, turnaround]
  //routeInfo = 1;
  byte loc = 0b0000000; 

  if (routeInfo != 0) {
    loc = routeInfo & 0b00001111; 
    // now have new information to update with
    //Serial.println(dir);
    //Serial.println(routeInfo);
    updateDirection(routeInfo);
    goToLoc(loc); 
     unsigned int positionPacket = byteifyCoordinate(currentX,currentY);
    // Position Packet as [XXXX-YYYY] 
    Serial.print(F("Dir = ")); Serial.println(dir);
    Serial.print(F("CurrentX = ")); Serial.println(currentX);
    Serial.print(F("CurrentY = ")); Serial.println(currentY);
    positionPacket = (positionPacket<<8) | ((maze[currentX][currentY]) & 0x00FF);
    printDfsStack(); 
    printPathStack(); 
    printMaze(); 

//    while (packetTransmission(positionPacket) == 0) {
//      packetTransmission(positionPacket);
//      //delay(300);
//    }
    //delay(300);no
//    Serial.println(F("(======================================="));
//     Serial.println(positionPacket);
//   Serial.println(F("======================================="));

  }
  bool dumStop = false; 
  while ((routeInfo & FRONTWALL) && (routeInfo & RIGHTWALL) && (routeInfo & LEFTWALL) || checkMazeEmpty()) { // checkMazeEmpty returns true if all nodes explored
    dumStop = true; 
    if (!dumStop){
    Serial.println(F("COMPLETE STOP"));
    bool dumStop = false;
    }
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
