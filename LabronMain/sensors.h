#ifndef SENSORS
#define SENSORS
#include <Arduino.h>

extern const int IRPinRight;
extern const int IRPinFront;
extern const int ForwardWallPin;
extern const int RightWallPin;

void setupIR();
void setupWallDetection();
int averageDistanceIRReading(int delayTime, int IRpin,int n);
int wallDetected();

#endif // SENSORS
