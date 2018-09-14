#include "servo_control.h"
#include <Arduino.h>

int vals[3];         // container for sensor output
//Outputs via the serial terminal – Lower numbers mean more reflected
const int QRE1113_Pin0 = A0; // connected to analog 0
const int QRE1113_Pin1 = A1; // connected to analog 1
const int QRE1113_Pin2 = A2; // connected to analog 2

// Check the IR sensor data
void checkSensors(int valsLocal []){
  // Read the IR sensor data
  valsLocal[0] = analogRead(QRE1113_Pin0);
  valsLocal[1] = analogRead(QRE1113_Pin1);
  valsLocal[2] = analogRead(QRE1113_Pin2);
}

// Write a speed to the servos
void servoSpeed(Servo leftServo, Servo rightServo, int leftSpeed, int rightSpeed){
  leftServo.write(leftSpeed);   // left Servo speed
  rightServo.write(rightSpeed); // right Servo speed
}

// Stop servo wheels
void straightenServo(Servo leftServo, Servo rightServo){
  // Finish turn by setting both servos to equal (0) speed
  leftServo.write(90);
  rightServo.write(90);
}

// Turn slightly left
void slightLeft(Servo leftServo, Servo rightServo){
  leftServo.write(60);
  rightServo.write(60);
  delay(50);
  straightenServo(leftServo, rightServo);
}

// Turn slightly right
void slightRight(Servo leftServo, Servo rightServo){
  leftServo.write(120);
  rightServo.write(120);
  delay(50);
  straightenServo(leftServo, rightServo);
}

// Turn left smoothly
void turnLeft(Servo leftServo, Servo rightServo){
  leftServo.write(90);
  rightServo.write(0);
  delay(600);
  checkSensors(vals);
  while(vals[1] > 200)
  {
    checkSensors(vals);
  }
  rightServo.write(90);
}

// Turn right smoothly
void turnRight(Servo leftServo, Servo rightServo){
  leftServo.write(180);
  rightServo.write(90);
  delay(600);
  checkSensors(vals);
  while(vals[1] > 200)
  {
    checkSensors(vals);
  }
  leftServo.write(90);
}

// Move to the intersection for right angle turns
void moveToIntersection(Servo leftServo, Servo rightServo){
  // move forward a little bit so that LeBron can turn
  leftServo.write(180);
  rightServo.write(0);
  delay(350);
  leftServo.write(90);
  rightServo.write(90);
}

