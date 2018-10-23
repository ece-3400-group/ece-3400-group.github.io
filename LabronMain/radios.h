#ifndef RADIOS_H
#define RADIOS_H
#include <Arduino.h>
#include "RF24.h"

#include <SPI.h>
#include "nRF24L01.h"

#define RADIO_DEBUG_LED 2

void setupRadios();
byte packetTransmission(int positionPacket, byte DEBUG=1);

#endif // RADIOS_H
