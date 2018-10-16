#ifndef SERVO_CONTROL
#define SERVO_CONTROL
#include <Servo.h>
#include <Arduino.h>

extern const int QRE1113_Pin0; // connected to analog 0
extern const int QRE1113_Pin1; // connected to analog 1
extern const int QRE1113_Pin2;

void setupLineSensors();
void setupServos();
void readServos(byte debug=1);

void checkSensors();

void turnLeft();
void turnRight();
void goStraight();
void stop();
void slightRight();
void slightLeft();

void turnAround();

void decideRoute();


#endif // SERVO_CONTROL
