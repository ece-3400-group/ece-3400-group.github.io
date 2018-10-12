#ifndef SENSORS
#define SENSORS

void setupIR();
int averageDistanceIRReading(int delayTime, int IRpin,int n);
int wallDetected();

#endif // SENSORS
