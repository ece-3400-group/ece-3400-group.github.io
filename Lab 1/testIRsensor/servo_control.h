#ifndef SERVO_CONTROL
#define SERVO_CONTROL
#include <Servo.h>

extern const int QRE1113_Pin0; // connected to analog 0
extern const int QRE1113_Pin1; // connected to analog 1
extern const int QRE1113_Pin2;
extern int vals[];

void checkSensors(int valsLocal []);
void servoSpeed(Servo leftServo, Servo rightServo, int leftSpeed, int rightSpeed);
void straightenServo(Servo leftServo, Servo rightServo);
void slightLeft(Servo leftServo, Servo rightServo);
void slightRight(Servo leftServo, Servo rightServo);
void adjustRoute(Servo leftServo, Servo rightServo, int vals[]);
void turnLeft(Servo leftServo, Servo rightServo);
void turnRight(Servo leftServo, Servo rightServo);
void moveToIntersection(Servo leftServo, Servo rightServo);

#endif // SERVO_CONTROL
