#ifndef RADIOS
#define RADIOS
#include <Arduino.h>

// TODO: EXTERN CONSTS based on Lab3/sendMazeInfo.ino

void setupRadios();
void packetTransmission(byte positionPacket, byte metaPacket);


#endif // SENSORS
