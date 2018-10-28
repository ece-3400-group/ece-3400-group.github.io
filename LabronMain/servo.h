#ifndef SERVO_CONTROL
#define SERVO_CONTROL
#include <Servo.h>
#include <Arduino.h>

/*==================== [ Pin Definitions ] ===============================*/
#define RIGHTSERVO_PIN 5
#define LEFTSERVO_PIN  6
//Outputs via the serial terminal – Lower numbers mean more reflected
#define QRE1113_PIN0 A0
#define QRE1113_PIN1 A1
#define QRE1113_PIN2 A2

/*==================== [ Threshold Values ] ===============================*/
#define LINE_THRESHOLD 200

/*==================== [ Bit Masks ] ===============================*/
 // walldir now contains wall information in its first 4 bits, and new direction information in its last 4 bits
 // first four bits organized [F, R, B[ehind], L] and are raised high when a wall is present
 // next four bits organized {forward, right, left, turnaround} to be interpreted in main loop
#define FORWARD 0b00001000
#define RIGHT   0b000000100
#define FLIP    0b00000010
#define LEFT    0b00000001

/*==================== [ Function Prototypes] ===============================*/
// Line Sensors
void setupLineSensors();
void checkSensors();

// Servo Control
void setupServos();
void readServos(byte debug=1);

// Movement
void turnLeft();
void turnRight();
void goStraight();
void stop();
void slightRight();
void slightLeft();
void turnAround();

// Ultimate Function
byte decideRoute();


#endif // SERVO_CONTROL
