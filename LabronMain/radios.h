#ifndef RADIOS_H
#define RADIOS_H
#include <Arduino.h>

void setupRadios();
void packetTransmission(byte positionPacket, byte metaPacket);

#endif // RADIOS_H
