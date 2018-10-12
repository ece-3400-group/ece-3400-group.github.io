#ifndef SERVO_CONTROL
#define SERVO_CONTROL
#include <Servo.h>
#include <Arduino.h>

extern const int QRE1113_Pin0; // connected to analog 0
extern const int QRE1113_Pin1; // connected to analog 1
extern const int QRE1113_Pin2;

void setupLineSensors();
void setupServos();

void checkSensors();
void readServos(byte debug=1);
void servoSpeed(int leftSpeed, int rightSpeed);
void straightenServo();
void slightLeft();
void slightRight();
void adjustRoute();
void turnLeft();
void turnRight();
void goStraight();
void moveToIntersection();

void decideRoute();
void adjustRoute();

#endif // SERVO_CONTROL
