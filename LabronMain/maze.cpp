
/* File to keep track of internal maze state and direction */

#include "maze.h"
int currentX = 0;
int currentY = 2;
byte direction = 0b01; // east
byte maze[MAZEX][MAZEY];

// DIRECTIONS
// 0b01 = EAST, 0b00 = NORTH, 0b10 = WEST, 0b11 = SOUTH
void populateMazeStart() {
  for (int i = 0; i < MAZEX; i++) {
    for (int j = 0; j < MAZEY; j++ ) {
      //maze[i][j] = 0b00000000;
      maze[i][j] = 0b11111111;
    }
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


void updateMaze(int x, int y, byte metaPacket){

  maze[x][y] = metaPacket;
}

byte getMazeInfo(int x, int y){
  return maze[x][y];
}

void updateDirection(byte decidedRoute){ // decidedRoute is output of decidedRoute
  // bits 4-7 = wall F(orward)R(ight)B(ehind)L(eft) 1 hot-encoded
  // bits 0-3 = which direction to go [F|R|B|L] like above
  // The packet returned by decidedRoute is relative to LABron's current direction.
  // We need to define walls and other info for the global map with absolute cardinal directions.

  byte metaPacket = 0b00000000;//maze[currentX][currentY]; // current value at (currentX, currentY)
  /*
  Description of metaPacket:
    [7] - Wall in Front
    [6] - Wall to Right
    [5] - Wall Behind
    [4] - Wall to Left
    [3:0] - Encoding of Treasures and Colors!
          0000 - No Treasure
          0001 - Red Circle
          0010 - Red Triangle
          0011 - Red Square
          1001 - Blue Circle
          1010 - Blue Triangle
          1011 - Blue Square
  */

  // In the future have metaPacket take on value of maze[currentX][currentY] if maze[currentX][currentY] does not equal 0b111111111 (otherwise, metaPacket = 0b0)

  // First using the current position and direction, define walls at the intersection
  int wallForward = bitRead(decidedRoute,7);
  int wallRight = bitRead(decidedRoute,6);
  int wallLeft = bitRead(decidedRoute,4);
  int wallBehind = bitRead (decidedRoute,5);
  if (direction == 0b01){ // facing EAST
    if (wallForward == 1){
      // East wall at x,y
      metaPacket |= 0b00100000;
    }
    if (wallRight == 1){
      // North wall at x,y
      metaPacket |= 0b00010000;
    }
    if (wallLeft == 1){
      // South wall at x,y
      metaPacket |= 0b01000000;
    }
    if (wallBehind ==1){
      // West wall at x,y
      metaPacket |= 0b10000000;
    }
  }
  else if (direction == 0b11){ // facing SOUTH
    if (wallForward == 1){
      // South wall at x,y
      metaPacket |= 0b00010000;
    }
    if (wallRight == 1){
      // West wall at x,y
      metaPacket |= 0b10000000;
    }
    if (wallLeft == 1){
      // East wall at x,y
      metaPacket |= 0b00100000;
    }
    if (wallBehind ==1){
      // North wall at x,y
      metaPacket |= 0b01000000;
    }
  }
  else if (direction == 0b10){ // facing WEST
    if (wallForward == 1){
      // West wall at x,y
      metaPacket |= 0b10000000;
    }
    if (wallRight == 1){
      // North wall at x,y
      metaPacket |= 0b01000000;
    }
    if (wallLeft == 1){
      // South wall at x,y
      metaPacket |= 0b00010000;
    }
    if (wallBehind ==1){
      // East wall at x,y
      metaPacket |= 0b00100000;
    }
  }
  else{ // facing NORTH
    if (wallForward == 1){
      // North wall at x,y
      metaPacket |= 0b01000000;
    }
    if (wallRight == 1){
      // East wall at x,y
      metaPacket |= 0b00100000;
    }
    if (wallLeft == 1){
      // West wall at x,y
      metaPacket |= 0b10000000;
    }
    if (wallBehind ==1){
      // South wall at x,y
      metaPacket |= 0b00010000;
    }
  }

  // Now update the global direction using the last four bits
  int forward = bitRead(decidedRoute,3);
  int right = bitRead(decidedRoute,2);
  int left = bitRead(decidedRoute,0);
  int flip = bitRead (decidedRoute,1); // turn around
  if (forward+right+left+flip != 1){
    //Serial.println("Multiple directions specified. Check logic");
  }

  // TODO: update current position based on direction
  if (direction == 0b00){
    // Going North (positive Y direction)
    currentX = currentX;
    currentY = currentY + 1;
  }
  if (direction == 0b11){
    // Going South (negative Y direction)
    currentX = currentX;
    currentY = currentY - 1;
  }
  else if (direction == 0b01){
    // Going East (positive X direction)
    currentX = currentX + 1;
    currentY = currentY;
  }
  else if (direction == 0b10){
    // Going West (negative X direction)
    currentX = currentX - 1;
    currentY = currentY;
  }
  else{
    // Don't change position
    currentX = currentX;
    currentY = currentY;
  }

  if (right == 1){
    if (direction == 0b00) direction = 0b01;
    else if (direction == 0b01) direction = 0b11;
    else if (direction == 0b10) direction = 0b00;
    else if (direction == 0b11) direction = 0b10;
    else{
      //Serial.println ("Error at direction update logic");
    }
    Serial.print(F("direction: "));Serial.println(direction);
  }

  else if (left == 1){
    if (direction == 0b00) direction = 0b10;
    else if (direction == 0b01) direction = 0b00;
    else if (direction == 0b10) direction = 0b11;
    else if (direction == 0b11) direction = 0b01;
    else{
      //Serial.println ("Error at direction update logic");
    }
  }
  else if (flip == 1){
    direction = ~direction;
  }
  else{
    // Forward and do nothing to direction
    direction = direction; // redundant, included for clarity
  }
  updateMaze(currentX, currentY, metaPacket);
}
