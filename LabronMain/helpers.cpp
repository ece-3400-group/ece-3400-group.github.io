#include "helpers.h"
#include <Arduino.h>



// Invert the number for the servo to make the 0 - 90 range logic intuitive
int invert(int number){
  return abs(number-180);
}

void serialBegin( int baudrate ) {
  Serial.begin( baudrate );
}
