#ifndef RADIOS
#define RADIOS
#include <Arduino.h>

// TODO: EXTERN CONSTS based on Lab3/sendMazeInfo.ino

void setupRadios();
unsigned long packetTransmission(byte positionPacket, byte metaPacket);


#endif // SENSORS
