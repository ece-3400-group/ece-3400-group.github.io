#include "helpers.h"
#include <Arduino.h>



// Invert the number for the servo to make the 0 - 90 range logic intuitive
int invert(int number){
  return abs(number-180);
}

byte byteifyCoordinate(int x, int y){
  return ((x<<4|y) & (0x00FF)); 
}
void serialBegin( int baudrate ) {
  Serial.begin( baudrate );
}


int decodePositionByte(byte xxxxyyyy) {
  // bits 7-4 are x, 3-0 are y as this is written. Check this please.
  int x = (xxxxyyyy & 0b11110000) >> 4; // AND-ed in case it wraps around or some wack shit p sure it doesn't tho
  int y = (xxxxyyyy & 0b00001111) << 4;
  return (10 * x + y); // x should be int-divided by 10, y should be %10

}
