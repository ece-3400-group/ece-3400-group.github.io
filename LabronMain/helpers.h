#ifndef HELPERS
#define HELPERS
#include <Arduino.h>

int invert(int number);
void serialBegin( int baudrate );
byte byteifyCoordinate(int x, int y); 
int decodePositionByte(byte xxxxyyyy); 

#endif // HELPERS
