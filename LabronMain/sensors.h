#ifndef SENSORS
#define SENSORS
#include <Arduino.h>

/******************* [ Threshold Values ] **************************/
#define wallThreshold 200
#define wallDetectedDelay 30
#define wallDetectedAverage 8

/******************* [ Pin Definitions ] **************************/
#define IRFRONT_PIN     A3
#define IRLEFTRIGHT_PIN A4
#define IRMUX_PIN       8
#define IRLEFT          0
#define IRRIGHT         1
#define RIGHTWALL_PIN   3
#define FORWARDWALL_PIN 4

/******************* [ Bit Definitions ] **************************/
// returning encoded wall packet
  // encoding goes (F, R, B[ehind], L, nullx4 so for example if wall was in front then first bit is high all others are 0
#define FRONTWALL 0b10000000
#define RIGHTWALL 0b01000000
#define LEFTWALL  0b00010000
#define NOWALL 0b00000000

/******************* [ Function Prototypes ] **************************/
void setupIR();
void setupWallDetection();
int averageDistanceIRReading(int delayTime, int IRpin,int n);
byte wallDetected();

#endif // SENSORS
