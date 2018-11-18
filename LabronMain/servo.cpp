#include "servo.h"
#include <Arduino.h>
#include "sensors.h"
#include "helpers.h"
/*==================== [ Initialize Servos ] ================in===============*/
Servo rightServo;
Servo leftServo;
<stack> dfsStack;
# define FORWARDDIR 0b00001000;
# define LEFTDIR 0b00000001;
# define RIGHTDIR 0b00000100;
# define FLIPDIR 0b00000010;
// 0b00000100 right
// 0b00000010 flip
// 0b00000001 left
int vals[3];         // container for sensor output

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
    Serial.print(F("Rightt Servo = [ ") );
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
  delay(600);
  checkSensors();
  while (vals[1] > LINE_THRESHOLD)
  {
    checkSensors();
  }
  rightServo.write(90);
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
  // if (direction == 0b00){
  //   direction = 0b01;
  // }
  // elif (direction == 0b01){
  //   direction = 0b11;
  // }
  // elif (direction == 0b11){
  //   direction = 0b10;
  // }
  // else (direction == 0b10){
  //   direction = 0b00;
  // }d
}

//Turn around
void turnAround() {
  leftServo.write(180);
  rightServo.write(0);
  delay(400);
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

void decodePositionByte(byte xxxxyyyy) {
  // bits 7-4 are x, 3-0 are y as this is written. Check this please.
  int x = (xxxxyyyy & 0b11110000) >> 4; // AND-ed in case it wraps around or some wack shit p sure it doesn't tho
  int y = (xxxxyyyy & 0b00001111) << 4;
  return (10 * x + y); // x should be int-divided by 10, y should be %10

}


void dfsPath(byte walldir) {
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
  if (direction == 0b01) { // facing EAST right now @Hojung does this need to be imported
    if (walldir & FRONT) {
      wallEast = true;
    }
    if (walldir & RIGHT) {
      wallSouth = true;
    }
    if (walldir & LEFT) {
      wallNorth = true;
    }
  }
  else if (direction == 0b00) { // facing NORTH right now
    if (walldir & FRONT) {
      wallNorth = true;
    }
    if (walldir & RIGHT) {
      wallEast = true;
    }
    if (walldir & LEFT) {
      wallWest = true;
    }
  }

  else if (direction == 0b10) { // facing WEST right now
    if (walldir & FRONT) {
      wallWest = true;
    }
    if (walldir & RIGHT) {
      wallNorth = true;
    }
    if (walldir & LEFT) {
      wallSouth = true;
    }
  }

  else if (direction == 0b11) { // facing SOUTH right now
    if (walldir & FRONT) {
      wallSouth = true;
    }
    if (walldir & RIGHT) {
      wallWest = true;
    }
    if (walldir & LEFT) {
      wallEast = true;
    }
  }
  else { // shouldn't ever happen
    stop();
  }




}
else { // TODO: Check if this is right
  stop();
}
// push neighboring locations to DFS Stack if there's no wall

if (!wallEast && maze[currentX + 1][currentY] != 0b11111111) {
  dfsStack.push(byteifyCoordinate(currentX + 1, currentY));
}
if (!wallSouth && maze[currentX][currentY - 1] != 0b11111111) {
  dfsStack.push(byteifyCoordinate(currentX, currentY - 1));
}
if (!wallNorth && maze[currentX][currentY + 1] != 0b11111111) {
  dfsStack.push(byteifyCoordinate(currentX, currentY + 1));
}
if (!wallWest && maze[currentX - 1][currentY] != 0b11111111) {
  dfsStack.push(byteifyCoordinate(currentX - 1, currentY));
}

byte nextLocByte(int nextX, int nextY)
{
  // With a known currentX, currentY, which action (right turn, left turn, forward, flip) should LABron pass
  // to get to position (nextX,nextY).
  // 0b00001000 forward
  // 0b00000100 right
  // 0b00000010 flip
  // 0b00000001 left

  if (nextX == currentX + 1) { // need to go EAST
    if (direction == 0b00) { // left turn
      return LEFTDIR;
    }
    else if (direction == 0b11) { // right turn
      return RIGHTDIR;
    }
    else if (direction == 0b10) { // flip
      return FLIPDIR;
    }
    else if (direction == 0b01) { //forward
      return FORWARDDIR;
    }
  }
  else if (nextX == currentX - 1) { // need to go WEST
    if (direction == 0b00) { // right turn
      return RIGHTDIR;
    }
    else if (direction == 0b11) { // left turn
      return LEFTDIR;
    }
    else if (direction == 0b10) { // forward
      return FORWARDDIR;
    }
    else if (direction == 0b01) { //flip
      return FLIPDIR;
    }
  }
  else if (nextY == currentY + 1) { // need to go NORTH
    if (direction == 0b00) { // forward
      return FORWARDDIR;
    }
    else if (direction == 0b11) { // flip
      return FLIPDIR;
    }
    else if (direction == 0b10) { // right turn
      return RIGHTDIR;
    }
    else if (direction == 0b01) { // left turn
      return LEFTDIR;
    }
  }
  else if (nextY == currentY + 1) { // need to go SOUTH
    if (direction == 0b00) { // flip
      return FLIPDIR;
    }
    else if (direction == 0b11) { // forward
      return FORWARDDIR;
    }
    else if (direction == 0b10) { // right turn
      return RIGHTDIR;
    }
    else if (direction == 0b01) { // left turn
      return LEFTDIR;
    }
  }
  else {
    return FLIPDIR; // turn around because you're confused
  }
}

byte decideRouteDFS() {
  checkSensors(); // still need old sensory check
  Serial.println(vals[1]);  // Taking out all the print statements makes LABron act weird im leaving one in -R
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
    dfsPath(walldir); // update dfsStack based on wallDir, returns nothing
    int nextLoc = decodePositionByte(dfsStack.pop()); // based on the most recent popped thing choose nextLoc and move there
    int nextX = nextLoc / 10; // nextLoc byte encoded as concatenated xy int so our maze can't handle over 9x9 mazes (which is chill lol)
    int nextY = nextLoc % 10;
    walldir |= nextLocByte(nextX, nextY); // now choose which direction nextX, nextY corresponds to (original byte encoding Forward-Right-Backward-Left)
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

bool checkMazeEmpty() {
  for (int i = 0; i < MAZEX; i++) {
    for (int j = 0; j < MAZEY; j++) {
      if (maze[i][j] == 0b11111111) { // there is an unexplored node still
        return false;
      }
    }
  }
  return true; // THE ENTIRE MAZE IS EXPLORED
}
// Logic for deciding the route for the robot
byte decideRoute() {
  // uses right hand wall following
  checkSensors();

  Serial.println(vals[1]);  // Taking out all the print statements makes LABron act weird im leaving one in -R
  int leftSpeed;
  int rightSpeed;
  byte direction = 0b00000000;


  if (vals[0] > LINE_THRESHOLD && vals[2] > LINE_THRESHOLD && vals[1] < LINE_THRESHOLD) { //go straight
    //Serial.println("NOTHING");
    goStraight();
    return 0;          // No intersection reached, so return 0 (no radio information necessary)
  }

  else if (vals[0] < LINE_THRESHOLD && vals[1] < LINE_THRESHOLD && vals[2] < LINE_THRESHOLD)  //intersection initiate turn
  {
    stop();
    byte walldir = wallDetected();

    // bits [7-4] are wall directions based on sensors, bits [3-0] are the directions we choose to go
    // bit[3] forward, bit[2]  right, bit[1] turn aroudn, bit[0] left, all 0 do nothing
    if (walldir == FRONT || walldir == NOWALL || walldir == (FRONT | LEFT)) {
      // No wall detected to right, so turn right
      Serial.println("No Wall");
      walldir = walldir | 0b000000100;
      turnRight();
      digitalWrite(RIGHTWALL_PIN, LOW);
      digitalWrite(FORWARDWALL_PIN, LOW);
    }


    else if ((walldir & FRONT) && (walldir & RIGHT) && (walldir & LEFT)) {
      // Wall detected to right, front,and left so turn around
      // walldir is 11010000
      digitalWrite(RIGHTWALL_PIN, HIGH);
      digitalWrite(FORWARDWALL_PIN, HIGH);
      // digitalWrite(LEFTWALL_PIN, HIGH); // not sure if this LED exists
      walldir = walldir | 0b00000010; // bit 1 corresponds to flipping direction
      //turnAround();
      stop();
      Serial.println("Wall to FRONT and RIGHT and LEFT");
      digitalWrite(RIGHTWALL_PIN, LOW);
      digitalWrite(FORWARDWALL_PIN, LOW);
    }
    else if ((walldir & FRONT) && (walldir & RIGHT)) {
      // Wall detected to right AND in front, so turn left
      digitalWrite(RIGHTWALL_PIN, HIGH);
      digitalWrite(FORWARDWALL_PIN, HIGH);
      walldir = walldir | 0b00000001;
      turnLeft();
      Serial.println("Wall to RIGHT and FRONT");
      digitalWrite(RIGHTWALL_PIN, LOW);
      digitalWrite(FORWARDWALL_PIN, LOW);
    }
    else if (walldir & RIGHT) {
      // Wall detected to right, but NOT in front, so move forward
      digitalWrite(RIGHTWALL_PIN, HIGH);
      digitalWrite(FORWARDWALL_PIN, LOW);
      walldir = walldir | 0b00001000;
      goStraight();
      Serial.println("Wall to RIGHT");
      while (vals[0] < LINE_THRESHOLD || vals[2] < LINE_THRESHOLD) {
        checkSensors();
      }
      stop();
    }
    else {
      Serial.println(F("Sorry something really weird happened"));
      stop();
    }
    return walldir;
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
