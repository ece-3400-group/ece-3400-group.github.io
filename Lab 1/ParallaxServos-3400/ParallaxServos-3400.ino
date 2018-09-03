#include <Servo.h>
  Servo rightServo;
  Servo leftServo;

int invert(int number){
  return abs(number-180);
}
  
void turnLeft(){
  int valLeft = 45;
  int valRight = 0;
  leftServo.write (valLeft);
  rightServo.write(valRight);
  delay(500);
}

void turnRight(){
  int valLeft = 0;
  int valRight = 45;
  leftServo.write (valLeft);
  rightServo.write(valRight);
  delay(500);
}

void straight(int pace){
  int valLeft = pace;
  int valRight = invert(pace);
  leftServo.write (valLeft);
  rightServo.write(valRight);
  delay(1000);
}

void figEight(int pace){
  straight(pace);
  turnLeft();
  straight(pace/2);
  turnLeft();
  straight(pace/2);
  turnLeft();
  turnRight();
  straight(pace/2);
  turnRight();
  straight(pace/2);
  turnRight();
}

void setup() {
  // put your setup code here, to run once:

  rightServo.attach(5);
  leftServo.attach(6);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  pinMode(A0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  figEight(50);
  delay(1000);
}


