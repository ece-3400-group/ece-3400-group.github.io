/* File to keep track of internal maze state and direction */

#include "maze.h"


void updateMaze(int x, int y, byte metaPacket){
  maze[x][y] = metaPacket;
}

byte getMazeInfo(int x, int y){
  return maze[x][y];
}

byte updateDirection(byte decidedRoute){ // decidedRoute is output of decidedRoute
  // bits 0-3 = wall F(orward)R(ight)B(ehind)L(eft) 1 hot-encoded
  // bits 4-7 = which direction to go (only 1 of these should be high)
  // The packet returned by decidedRoute is relative to LABron's current direction.
  // We need to define walls and other info for the global map with absolute cardinal directions.

  byte metaPacket = maze[currentX][currentY]; // current value at (currentX, currentY)

  // First using the current position and direction, define walls at the intersection
  int wallForward = bitRead(decidedRoute,4);
  int wallRight = bitRead(decidedRoute,5);
  int wallLeft = bitRead(decidedRoute,6);
  int wallSouth     = bitRead (decidedRoute,7); 
  if (direction == 0b01){ // facing EAST
    if (wallForward == 1){
      // North wall at x,y
      metaPacket |= 0b00100000;
    }
    if (wallRight == 1){
      // East wall at x,y
      metaPacket |= 0b00010000;
    }
    if (wallLeft == 1){
      // West wall at x,y
      metaPacket |= 0b01000000;
    }
    if (wallSouth ==1){
      // South wall at x,y
      metaPacket |= 0b00100000;
    }
  }
  else if (direction == 0b11){ // facing SOUTH
    if (wallForward == 1){
      // North wall at x,y
      metaPacket |= 0b00010000;
    }
    if (wallRight == 1){
      // East wall at x,y
      metaPacket |= 0b10000000;
    }
    if (wallLeft == 1){
      // West wall at x,y
      metaPacket |= 0b00100000;
    }
    if (wallSouth ==1){
      // South wall at x,y
      metaPacket |= 0b01000000;
    }
  }
  else if (direction == 0b10){ // facing WEST
    if (wallForward == 1){
      // North wall at x,y
      metaPacket |= 0b10000000;
    }
    if (wallRight == 1){
      // East wall at x,y
      metaPacket |= 0b01000000;
    }
    if (wallLeft == 1){
      // West wall at x,y
      metaPacket |= 0b00010000;
    }
    if (wallSouth ==1){
      // South wall at x,y
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
    if (wallSouth ==1){
      // South wall at x,y
      metaPacket |= 0b00010000;
    }
  }
  // Now update the global direction using the last four bits
  int forward = bitRead(decidedRoute,4);
  int right = bitRead(decidedRoute,5);
  int left = bitRead(decidedRoute,6);
  int behind = bitRead (decidedRoute,7); // how will it detect a wall behind it?
  if (forward+right+left+behind != 1){
    Serial.println("Multiple directions specified. Check logic");
  }

  if (right == 1){
    if (direction = 0b00) direction = 0b01;
    else if (direction = 0b01) direction = 0b11;
    else if (direction = 0b10) direction = 0b00;
    else if (direction = 0b11) direction = 0b10;
    else{
      Serial.println ("Error at direction update logic");
    }
  }

  else if (left == 1){
    if (direction = 0b00) direction = 0b10;
    else if (direction = 0b01) direction = 0b00;
    else if (direction = 0b10) direction = 0b11;
    else if (direction = 0b11) direction = 0b01;
    else{
      Serial.println ("Error at direction update logic");
    }
  }
  else if (behind == 1){
    direction = ~direction;
  }
  else{
    // Forward and do nothing to direction
    direction = direction; // redundant, included for clarity
  }
  // TODO: update current position based on direction
  if (direction == 0b00){
    currentX = currentX;
    currentY = currentY + 1;
  }
  else if (direction == 0b11){
    currentX = currentX;
    currentY = currentY - 1;
  }
  else if (direction == 0b01){
    currentX = currentX + 1;
    currentY = currentY;
  }
  else if (direction == 0b10){
    currentX = currentX - 1;
    currentY = currentY;
  }
  else{
    // Don't change position
    currentX = currentX;
    currentY = currentY;
  }
}
