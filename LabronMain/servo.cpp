#include "servo.h"
#include <Arduino.h>
#include "sensors.h"
#include "maze.h"
#include "helpers.h"
/*==================== [ Initialize Servos ] ================in===============*/
Servo rightServo;
Servo leftServo;

// 0b00000100 right
// 0b00000010 flip
// 0b00000001 left
int vals[3];         // container for sensor output
byte pathStack[MAZEY*MAZEX];  // at most MAZEY * MAZEX positions pushed, with head at stackSize and tail at 0
int pathStackSize = 0;
byte dfsStack[MAZEY*MAZEX];  // at most MAZEY * MAZEX positions pushed, with head at stackSize and tail at 0
int dfsStackSize = 0; // initialize dfStackSize to 0



void pathStackPush(byte pushed){

  if (pathStackSize > sizeof(pathStack)/sizeof(byte)){ // stack is too big
    Serial.println(F("PATHSTACK OVERFLOW >> ABORT!"));
    confused = true; 
  }
  else{
    bool seen = false;
    int seenIdx = -1;
    for (int i = 0; i < pathStackSize; ++i){
      if (pathStack[i] == pushed){
        seen = true;
        seenIdx = i;
      }
    }

    if (seen){
      Serial.println(F("Detected this node before! Swapping it to most current position."));
      for (int i=seenIdx; i<pathStackSize-1; i++){
        pathStack[i] = pathStack[i+1] ;
      }
    }
    // If seen, last element is "new" node, if not, then everything is the same
    Serial.print (F("Pushing "));
    Serial.print (decodePositionByte(pushed));
    Serial.println(F(" to PathStack"));
    if (!seen){
      pathStackSize = pathStackSize+1; // increment stack size only if unseen
      }
    pathStack [pathStackSize-1] = pushed;
  }
}


void dfsInit(byte initLoc){
  // If something goes wrong, we're going to clear stacks and restart exploration at the current location. This way, hopefully LABron is less confused all the time >:( 
  clearStacks(); 
  dfsStackPush(initLoc);
  pathStackPush(initLoc);
}

byte pathStackPop(){
  Serial.print (F("Popping "));
  Serial.println(F(" from PathStack"));
  if (pathStackSize<=0){
    Serial.println(F("PATHSTACK MUST BE POPULATED (POP)"));
    return 0b11111111; // nullish
    confused = true; 
  }
  else{
    byte returned = pathStack[pathStackSize-1];
    pathStackSize = pathStackSize - 1; // change address then pop the last array element (head)
    return returned;
  }
}

byte pathStackPeek(){
  Serial.print (F("Peeking "));
  Serial.println(F("from PathStack"));
  if (pathStackSize<=0){
    Serial.println(F("PATHSTACK MUST BE POPULATED (PEEK)"));
    return 0b11111111; // nullish
    confused = true; 
  }
  else{
    return pathStack[pathStackSize-1];
  }
}

void dfsStackPush(byte pushed){
  Serial.print (F("Pushing "));
  Serial.print (decodePositionByte(pushed));
  Serial.println(F(" to dfsStack"));
  if (dfsStackSize > sizeof(dfsStack)/sizeof(byte)){ // stack is too big
    Serial.println(F("DFS STACK OVERFLOW >> ABORT!"));
    confused = true; 
  }
  else{
    dfsStackSize=dfsStackSize + 1; // increment stack size
    dfsStack [dfsStackSize-1] = pushed;
  }
}

byte dfsStackPop(){
  Serial.print (F("Popping "));
  Serial.println(F(" from dfsStack"));
  if (dfsStackSize<=0){
    Serial.println("DFS STACK MUST BE POPULATED");
    confused = true; 
    return 0b11111111; // nullish
  }
  else{
    byte returned = dfsStack[dfsStackSize-1];
    dfsStackSize = dfsStackSize - 1; // change address then pop the last array element (head)
    return returned;
  }
}


byte dfsStackPeek(){
  Serial.print (F("Peeking "));
  Serial.println(F(" from dfsStack"));
  if (dfsStackSize<=0){
    Serial.println(F("PATHSTACK MUST BE POPULATED"));
    return 0b00000000; // nullish
    confused = true; 
  }
  else{
    return dfsStack[dfsStackSize-1];
  }
}

void setupLineSensors() {
  pinMode(QRE1113_PIN0, INPUT); // IR sensor 1
  pinMode(QRE1113_PIN1, INPUT); // IR sensor 2
  pinMode(QRE1113_PIN2, INPUT); // IR sensor 3
}

void setupServos() {
  // Attach the Servos to their corresponding pins
  rightServo.attach(RIGHTSERVO_PIN);
  leftServo.attach(LEFTSERVO_PIN);
}

void readServos(byte debug = 1) {
  int left = leftServo.read();
  int right = rightServo.read();
  if (debug) {
    Serial.print(F("Left Servo = [ " ));
    Serial.print( left );
    Serial.println( F(" ]"));
    Serial.print(F("Right Servo = [ ") );
    Serial.print( right ); Serial.println( F(" ]"));
  }
}


// Check the IR sensor data
void checkSensors() {
  // Read the IR sensor data
  vals[0] = analogRead(QRE1113_PIN0);  // Right sensor
  vals[1] = analogRead(QRE1113_PIN1);  // Middle Sensor
  vals[2] = analogRead(QRE1113_PIN2);  // Left Sensor

  //  Serial.print("right: ");Serial.println(vals[0]);
  //  Serial.print("mid: ");Serial.println(vals[1]);
  //  Serial.print("left: ");Serial.println(vals[2]);
  //  Serial.println("");
}

// Turn left
void turnLeft() {
  leftServo.write(90);
  rightServo.write(0);
  delay(500);
  checkSensors();
  while (vals[1] > LINE_THRESHOLD)
  {
    checkSensors();
  }
  stop();
}

// Turn slightly left
void slightLeft() {
  leftServo.write(90);
  rightServo.write(0);
  delay(50);
  goStraight();
}

// Turn slightly right
void slightRight() {
  leftServo.write(180);
  rightServo.write(90);
  delay(50);
  goStraight();
}

// Turn right
void turnRight() {
  leftServo.write(180);
  rightServo.write(90);
  delay(500);
  checkSensors();
  while (vals[1] > LINE_THRESHOLD)
  {
    checkSensors();
  }
  leftServo.write(90);
  // if (dir == 0b00){
  //   dir = 0b01;
  // }
  // elif (dir == 0b01){
  //   dir = 0b11;
  // }
  // elif (dir == 0b11){
  //   dir = 0b10;
  // }
  // else (dir == 0b10){
  //   dir = 0b00;
  // }d
  stop();
}

// Turn around

// Turn around
void turnAround() {
  leftServo.write(0);
  rightServo.write(0);
  delay(200);
  checkSensors();
  while (vals[1] > LINE_THRESHOLD) {
    checkSensors();
  }
  leftServo.write(0);
  rightServo.write(0);
  delay(200);
  checkSensors();
  while (vals[1] > LINE_THRESHOLD) {
    checkSensors();
  }
  stop();
}


// Move forwrard
void goStraight() {
  leftServo.write(100);
  rightServo.write(80);
}

// Stop moving
void stop() {
  leftServo.write(90);
  rightServo.write(90);
}


void printDfsStack(){

   Serial.println(F("\nPrinting DFS Stack"));
      Serial.print(F("Size: "));
      Serial.println (dfsStackSize);
   for (int i=0; i<MAZEY*MAZEX; i++){
      Serial.print (decodePositionByte(dfsStack[i]));
      Serial.print (" ");
   }
   Serial.println(F(""));
}
void printPathStack(){
   Serial.println(F("Printing PathStack"));
        Serial.print(F("Size: "));
      Serial.println (pathStackSize);
   for (int i=0; i<MAZEY*MAZEX; i++){
      Serial.print (decodePositionByte(pathStack[i]));
      Serial.print (F(" "));
   }
   Serial.println(F(""));

}

void goToLoc(byte nextLoc){
  if (nextLoc==RIGHT){
    turnRight();
  }
  else if (nextLoc==LEFT){
    turnLeft();
  }
  else if (nextLoc==FLIP){
    turnAround();
  }
  else if (nextLoc==FORWARD){
    goStraight();
    delay(400);
  }
}

bool dfsPath(byte walldir) {
  // use DFS and wall sensing to push to DFS stack. If there's no reachable unexplored locations push nothing
  checkSensors(); // still need old sensory check

  Serial.println(vals[1]);  // Taking out all the print statements makes LABron act weird im leaving one in -R
  int leftSpeed;
  int rightSpeed;


  // At an intersection


  bool wallSouth = false;
  bool wallNorth = false;
  bool wallEast = false;
  bool wallWest = false;
  // need to know absolute wall locations because DFS gives (absolute) coordinates
  if (dir == 0b01) { // facing EAST right now @Hojung does this need to be imported
    if (bitRead(walldir,7)) {
      wallEast = true;
    }
    if (bitRead(walldir,6)) {
      wallSouth = true;
    }
    if (bitRead(walldir,4)) {
      wallNorth = true;
    }
  }
  else if (dir == 0b00) { // facing NORTH right now
    if (bitRead(walldir,7)) {
      wallNorth = true;
    }
    if (bitRead(walldir,6)) {
      wallEast = true;
    }
    if (bitRead(walldir,4)) {
      wallWest = true;
    }
  }

  else if (dir == 0b10) { // facing WEST right now
    if (bitRead(walldir,7)) {
      wallWest = true;
    }
    if (bitRead(walldir,6)) {
      wallNorth = true;
    }
    if (bitRead(walldir,4)) {
      wallSouth = true;
    }
  }

  else if (dir == 0b11) { // facing SOUTH right now
    if (bitRead(walldir,7)) {
      wallSouth = true;
    }
    if (bitRead(walldir,6)) {
      wallWest = true;
    }
    if (bitRead(walldir,4)) {
      wallEast = true;
    }
  }

  else { // TODO: Check if this is right
    Serial.println(F("No dir. Check logic"));
  }
// push neighboring locations to DFS Stack if there's no wall
bool somethingPushed = false;

Serial.print(F("CurrentX: "));
Serial.println(currentX);
Serial.print(F("CurrentY: "));
Serial.println(currentY);
Serial.print (F("Wall East: "));
Serial.println(wallEast);
Serial.print (F("Wall West: "));
Serial.println(wallWest);
Serial.print (F("Wall North: "));
Serial.println(wallNorth);
Serial.print (F("Wall South: "));
Serial.println(wallSouth);

if (!(wallEast || wallWest || wallNorth || wallSouth)){
  Serial.println(F("No walls detected"));
}


if ((!wallEast) && (currentX + 1<MAZEX) && (maze[currentX + 1][currentY] == 0b11111111))
{
  pathStackPush(byteifyCoordinate(currentX + 1, currentY));
  somethingPushed = true;
  Serial.println (F("No wall EAST. Unexplored location at X + 1 pushed to PathStack"));
}
if ((!wallSouth) && (currentY - 1 >= 0) && (maze[currentX][currentY - 1] == 0b11111111)){
  pathStackPush(byteifyCoordinate(currentX, currentY - 1));
  somethingPushed = true;
  Serial.println (F("No wall SOUTH. Unexplored location at Y - 1 pushed to PathStack"));
  }
if ((!wallNorth) && (currentY + 1<MAZEY) && (maze[currentX][currentY + 1] == 0b11111111)) {
  pathStackPush(byteifyCoordinate(currentX, currentY + 1));
  somethingPushed = true;
  Serial.println (F("No wall NORTH. Unexplored location Y + 1 pushed to PathStack"));
  }
if ((!wallWest) && (currentX - 1 >= 0) && (maze[currentX - 1][currentY] == 0b11111111)) {
  pathStackPush(byteifyCoordinate(currentX - 1, currentY));
  somethingPushed = true;
   Serial.println (F("No wall WEST. Unexplored location at X - 1 pushed to PathStack"));
  }

return somethingPushed;
}
byte nextLocByte(int nextX, int nextY){
  Serial.print(F("CurrentX: "));
  Serial.println(currentX);
  Serial.print(F("CurrentY: "));
  Serial.println(currentY);
  Serial.print(F("NextX: "));
  Serial.println(nextX);
  Serial.print(F("NextY: "));
  Serial.println(nextY);

  // With a known currentX, currentY, which action (right turn, left turn, forward, flip) should LABron pass
  // to get to position (nextX,nextY).
  // 0b00001000 forward
  // 0b00000100 right
  // 0b00000010 flip
  // 0b00000001 left

  if (nextX == currentX + 1) { // need to go EAST
    if (dir == 0b00) {// Facing north // right turn
      return RIGHT;
    }
    else if (dir == 0b11) { // left turn
      return LEFT;
    }
    else if (dir == 0b10) { // flip
      return FLIP;
    }
    else if (dir == 0b01) { //forward
      return FORWARD;
    }
  }
  else if (nextX == currentX - 1) { // need to go WEST
    if (dir == 0b00) { // left turn
      return LEFT;
    }
    else if (dir == 0b11) { // right turn
      return RIGHT;
    }
    else if (dir == 0b10) { // forward
      return FORWARD;
    }
    else if (dir == 0b01) { //flip
      return FLIP;
    }
  }
  else if (nextY == currentY + 1) { // need to go NORTH
    if (dir == 0b00) { // forward
      return FORWARD;
    }
    else if (dir == 0b11) { // flip
      return FLIP;
    }
    else if (dir == 0b10) { // right turn
      return RIGHT;
    }
    else if (dir == 0b01) { // left turn
      return LEFT;
    }
  }
  else if (nextY == currentY - 1) { // need to go SOUTH
    if (dir == 0b00) { // flip
      return FLIP;
    }
    else if (dir == 0b11) { // forward
      return FORWARD;
    }
    else if (dir == 0b10) { // right turn
      return LEFT;
    }
    else if (dir == 0b01) { // left turn
      return RIGHT;
    }
  }
  else {
    Serial.println("nextLocByte confused");
    confused = true; 
    delay(1000);
    return FORWARD; // turn around because you're confused
  }
}

byte decideRouteDFS() {
  if (checkMazeEmpty()){
    Serial.println ("MAZE EXPLORATION DONE");
  }
  checkSensors(); // still need old sensory check
  delay(5);

  int leftSpeed;
  int rightSpeed;


  // not an intersection
  if (vals[0] > LINE_THRESHOLD && vals[2] > LINE_THRESHOLD && vals[1] < LINE_THRESHOLD) { //go straight
    //Serial.println("NOTHING");
    goStraight();
    return 0;          // No intersection reached, so return 0 (no radio information necessary)
  }
  // At an intersection
  else if (vals[0] < LINE_THRESHOLD && vals[1] < LINE_THRESHOLD && vals[2] < LINE_THRESHOLD) { //intersection initiate turn
    stop();
    byte walldir = wallDetected();
    Serial.print(F("Wall detection information ")); Serial.println(walldir);
    bool somethingPushed = dfsPath(walldir); // update pathStack based on wallDir, returns True if pathStack was pushed to (not dead end) otherwise false
    byte pathPeek;
    if (pathStackSize > 0){ // There's explorable space
      pathPeek = pathStackPeek();
    }
    if (!somethingPushed){
      Serial.println(F("TIME TO TURN AROUND!"));
      printPathStack();
      printDfsStack();
      byte pathPop = pathStackPop();
      byte dfsPop = dfsStackPop(); // trash variable for path stack, if nothing's pushed then the next location is the last location in both stacks
      if (pathPop != dfsPop){
        Serial.println(F("dfsStack is diverging from pathStack while retracing"));
        int nextLoc = decodePositionByte(dfsStackPeek());
        int nextX = nextLoc / 10;
        int nextY = nextLoc % 10;
        byte loc = nextLocByte(nextX, nextY);
        walldir |= loc;
        return walldir;
      }
      else{
        Serial.println (F("Retracing steps but not yet diverged"));
        int nextLoc = decodePositionByte(dfsStackPeek()); // based on the most recent popped thing choose nextLoc and move there
        int nextX = nextLoc / 10; // nextLoc byte encoded as concatenated xy int so our maze can't handle over 9x9 mazes (which is chill lol)
        int nextY = nextLoc % 10;
        byte loc = nextLocByte(nextX, nextY);
//        goToLoc(loc);
        walldir |= (loc); // now choose which dir nextX, nepxtY corresponds to (original byte encodging Forward-Right-Backward-Left)
        return walldir; // return this to LabronMain
      }
      // this dummy should equal dfsStackPop()
    }
    
    int nextLoc = decodePositionByte(pathPeek); // based on the most recent popped thing choose nextLoc and move there
    int nextX = nextLoc / 10; // nextLoc byte encoded as concatenated xy int so our maze can't handle over 9x9 mazes (which is chill lol)
    int nextY = nextLoc % 10;
    
    Serial.print(F("NextLoc: "));
    Serial.println(nextLoc);
    printDfsStack();
    printPathStack();
    byte loc = nextLocByte(nextX, nextY);
    dfsStackPush(pathPeek);
    walldir |= loc; // now choose which dir nextX, nextY corresponds to (original byte encoding Forward-Right-Backward-Left)
    return walldir; // return this to LabronMain


  }
  // Adjust the robot a bit if it's off the line
  else if (vals[2] < LINE_THRESHOLD) { //left sensor on line now adjust left
    //Serial.println("ADJUSTING LEFT");
    slightLeft();          // No intersection reached, so return 0 (no radio information necessary)
    return 0;
  }
  else if (vals[0] < LINE_THRESHOLD) { //right sensor on line now adjust right
    //Serial.println("ADJUSTING RIGHT");
    slightRight();          // No intersection reached, so return 0 (no radio information necessary)
    return 0;
  }

  // Stop the robot if it's off any white lines
  else { //stop
    //Serial.println("STOPPING");
    stop();                // No intersection reached, so return 0 (no radio information necessary)
    return 0;
  }
}


// Logic for deciding the route for the robot
byte decideRoute(bool right) {
  // uses right hand wall following
  checkSensors();

  Serial.println(vals[1]);  // Taking out all the print statements makes LABron act weird im leaving one in -R
  int leftSpeed;
  int rightSpeed;


  if (vals[0] > LINE_THRESHOLD && vals[2] > LINE_THRESHOLD && vals[1] < LINE_THRESHOLD) { //go straight
    //Serial.println("NOTHING");
    goStraight();
    return 0;          // No intersection reached, so return 0 (no radio information necessary)
  }

  else if (vals[0] < LINE_THRESHOLD && vals[1] < LINE_THRESHOLD && vals[2] < LINE_THRESHOLD)  //intersection initiate turn
  {
    stop();
    byte walldir = wallDetected();
    if (right){
    Serial.println(F("Right hand wall following")); 
    // bits [7-4] are wall dirs based on sensors, bits [3-0] are the dirs we choose to go
    // bit[3] forward, bit[2]  right, bit[1] turn aroudn, bit[0] left, all 0 do nothing
    if (walldir == FRONTWALL || walldir == NOWALL || walldir == (FRONTWALL | LEFTWALL)) {
      // No wall detected to right, so turn right
      Serial.println(F("No Wall to RIGHT"));
      walldir = walldir | 0b000000100;
//      turnRight();
//      digitalWrite(RIGHTWALL_PIN, LOW);
//      digitalWrite(FORWARDWALL_PIN, LOW);
    }


    else if ((walldir & FRONTWALL) && (walldir & RIGHTWALL) && (walldir & LEFTWALL)) {
      // Wall detected to right, front,and left so turn around
      // walldir is 11010000
//      digitalWrite(RIGHTWALL_PIN, HIGH);
//      digitalWrite(FORWARDWALL_PIN, HIGH);
      // digitalWrite(LEFTWALL_PIN, HIGH); // not sure if this LED exists
      walldir = walldir | 0b00000010; // bit 1 corresponds to flipping dir
      Serial.println(F("Wall to FRONT and RIGHT and LEFT"));
//      turnAround();
//      digitalWrite(RIGHTWALL_PIN, LOW);
//      digitalWrite(FORWARDWALL_PIN, LOW);
    }
    else if ((walldir & FRONTWALL) && (walldir & LEFTWALL)) {
      // Wall detected to right AND in front, so turn left
//      digitalWrite(RIGHTWALL_PIN, HIGH);
//      digitalWrite(FORWARDWALL_PIN, HIGH);
      walldir = walldir | 0b00000100;
//      turnRight();
      Serial.println(F("Wall to LEFT and FRONT"));
//      digitalWrite(RIGHTWALL_PIN, LOW);
//      digitalWrite(FORWARDWALL_PIN, LOW);
    }
    else if (walldir & LEFTWALL) {
      // Wall detected to left, but NOT in front, so move forward
//      digitalWrite(RIGHTWALL_PIN, HIGH);
//      digitalWrite(FORWARDWALL_PIN, LOW);
      walldir = walldir | 0b00001000;
//      goStraight();
      Serial.println(F("Wall to LEFT"));
      while (vals[0] < LINE_THRESHOLD || vals[2] < LINE_THRESHOLD) {
        checkSensors();
      }
      stop();
    }
    else {
      Serial.println(F("Sorry something really weird happened"));
      confused = true; 
    }
    return walldir;
  }
  else{ //left hand wall following
    Serial.println(F("Left hand wall following")); 
      // bits [7-4] are wall dirs based on sensors, bits [3-0] are the dirs we choose to go
    // bit[3] forward, bit[2]  right, bit[1] turn aroudn, bit[0] left, all 0 do nothing
    if (walldir == FRONTWALL || walldir == NOWALL || walldir == (FRONTWALL | RIGHTWALL)) {
      // No wall detected to right, so turn left
      Serial.println(F("No Wall to LEFT"));
      walldir = walldir | 0b000000001;
      turnLeft();
//      digitalWrite(RIGHTWALL_PIN, LOW);
//      digitalWrite(FORWARDWALL_PIN, LOW);
    }


    else if ((walldir & FRONTWALL) && (walldir & RIGHTWALL) && (walldir & LEFTWALL)) {
      // Wall detected to right, front,and left so turn around
      // walldir is 11010000
//      digitalWrite(RIGHTWALL_PIN, HIGH);
//      digitalWrite(FORWARDWALL_PIN, HIGH);
      // digitalWrite(LEFTWALL_PIN, HIGH); // not sure if this LED exists
      walldir = walldir | 0b00000010; // bit 1 corresponds to flipping dir
      turnAround();
      Serial.println(F("Wall to FRONT and RIGHT and LEFT"));
//      digitalWrite(RIGHTWALL_PIN, LOW);
//      digitalWrite(FORWARDWALL_PIN, LOW);
    }
    else if ((walldir & FRONTWALL) && (walldir & RIGHTWALL)) {
      // Wall detected to right AND in front, so turn left
//      digitalWrite(RIGHTWALL_PIN, HIGH);
//      digitalWrite(FORWARDWALL_PIN, HIGH);
      walldir = walldir | 0b00000001;
      turnLeft();
      Serial.println(F("Wall to RIGHT and FRONT"));
//      digitalWrite(RIGHTWALL_PIN, LOW);
//      digitalWrite(FORWARDWALL_PIN, LOW);
    }
    else if (walldir & RIGHTWALL) {
      // Wall detected to right, but NOT in front, so move forward
//      digitalWrite(RIGHTWALL_PIN, HIGH);
//      digitalWrite(FORWARDWALL_PIN, LOW);
      walldir = walldir | 0b00001000;
      goStraight();
      Serial.println(F("Wall to RIGHT"));
      while (vals[0] < LINE_THRESHOLD || vals[2] < LINE_THRESHOLD) {
        checkSensors();
      }
      stop();
    }
    else {
      Serial.println(F("Sorry something really weird happened"));
      confused = true; 
    }
    return walldir;
  }
  }

  // Adjust the robot a bit if it's off the line
  else if (vals[2] < LINE_THRESHOLD) { //left sensor on line now adjust left
    //Serial.println("ADJUSTING LEFT");
    slightLeft();          // No intersection reached, so return 0 (no radio information necessary)
    return 0;
  }
  else if (vals[0] < LINE_THRESHOLD) { //right sensor on line now adjust right
    //Serial.println("ADJUSTING RIGHT");
    slightRight();          // No intersection reached, so return 0 (no radio information necessary)
    return 0;
  }

  // Stop the robot if it's off any white lines
  else { //stop
    //Serial.println("STOPPING");
    stop();                // No intersection reached, so return 0 (no radio information necessary)
    return 0;
  }
}

void clearStacks(){
  for (int i=0; i<MAZEY*MAZEX; i++){
    pathStack[i] = 0b11111111;
    dfsStack[i] = 0b11111111;
  }
}
