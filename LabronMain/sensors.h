#ifndef SENSORS
#define SENSORS
#include <Arduino.h>

extern const int IRPinRight;
extern const int IRPinFront;

void setupIR();
void setupWallDetection();
int averageDistanceIRReading(int delayTime, int IRpin,int n);
int wallDetected();

#endif // SENSORS
