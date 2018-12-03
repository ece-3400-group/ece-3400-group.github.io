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
  byte firstByte = byteifyCoordinate(currentX,currentY); // Initial starting point stack push
  dfsInit(firstByte); 
//  while( waitForStart() );
}

int count = 0;
int confusedCount = 0; 
void loop() {
  if (checkMazeEmpty()){
    Serial.println(F("MAZE IS EXPLORED LEBRON IS DONE"));
  }
  byte routeInfo; 
  if (confused){
          Serial.print ("Labron is confused. It's time to switch to randomish wall following"); 
          confusedCount = confusedCount + 1; 
          if (confusedCount<5){
            byte currentLoc = byteifyCoordinate(currentX,currentY); 
            dfsInit(currentLoc); 
            confused = false; 
            routeInfo = decideRouteDFS();

          }
          else{ // DFS probably broken, switch to randomly wall following
                bool wallFollow = ((confusedCount%2==0)? true:false);  // right hand wall follow if even, left hand wall follow if odd
                routeInfo = decideRoute(wallFollow);  
                
          }
  }
  else{
          routeInfo = decideRouteDFS();
  }
  // routeInfo organized [F,R,B,L; forward, right, left, turnaround]
  //routeInfo = 1;
  byte loc = 0b0000000;
  
  if (routeInfo != 0) {
    Serial.println(F("Confusion: "));
    Serial.print(confused); 
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

    while (packetTransmission(positionPacket) == 0) {
      packetTransmission(positionPacket);
      delay(300);
    }
    delay(300);
    Serial.println(F("(======================================="));
    Serial.println(positionPacket);
    Serial.println(F("======================================="));

  }
  bool dumStop = false;
  while ((routeInfo & FRONTWALL) && (routeInfo & RIGHTWALL) && (routeInfo & LEFTWALL) || checkMazeEmpty()) { // checkMazeEmpty returns true if all nodes explored
    dumStop = true;
    if (!dumStop){
    Serial.println(F("COMPLETE STOP"));
    bool dumStop = false;
    }
    if (checkMazeEmpty()){
      Serial.println(F("Maze completed!")); 
    }
    stop();
  }
  if (count == 0){
   // debugFFT();
    while (fftResult & readFFT(ADC5_FFT) & IRHAT_MASK) {
      Serial.println("IR HAT DETECTED");
      fftResult = 0;
      confused = true;
      reverse(); 
    }
    fftResult = readFFT(ADC5_FFT);
  }
  //displayLedFFT(fftResult, AUDIO_PIN, IRHAT_PIN );
  count++;
  count = count % 3;
}
