#include "servo.h"
#include <Arduino.h>
#include "sensors.h"

/*==================== [ Initialize Servos ] ================in===============*/
Servo rightServo;
Servo leftServo;

/*==================== [ Assign Pins ] ===============================*/
const int RIGHT_PIN = 5; // Right servo control pin
const int LEFT_PIN  = 6; // Left servo control pin
//Outputs via the serial terminal – Lower numbers mean more reflected
const int QRE1113_Pin0 = A0; // connected to analog 0
const int QRE1113_Pin1 = A1; // connected to analog 1
const int QRE1113_Pin2 = A2; // connected to analog 2

int vals[3];         // container for sensor output

void setupLineSensors() {
  pinMode(QRE1113_Pin0,INPUT);  // IR sensor 1
  pinMode(QRE1113_Pin1,INPUT);  // IR sensor 2
  pinMode(QRE1113_Pin2,INPUT);  // IR sensor 3
}

void setupServos() {
    // Attach the Servos to their corresponding pins
  rightServo.attach(RIGHT_PIN);
  leftServo.attach(LEFT_PIN);
}

void readServos(byte debug=1) {
  int left = leftServo.read();
  int right = rightServo.read();
  if (debug) {
    Serial.print( "Left Servo = [ " ); Serial.print( left ); Serial.println( " ]");
    Serial.print( "Rightt Servo = [ " ); Serial.print( right ); Serial.println( " ]");
  }
}

// Check the IR sensor data
void checkSensors(){
  // Read the IR sensor data
  vals[0] = analogRead(QRE1113_Pin0);
  vals[1] = analogRead(QRE1113_Pin1);
  vals[2] = analogRead(QRE1113_Pin2);
}

// Turn left
void turnLeft(){
  leftServo.write(90);
  rightServo.write(0);
  delay(600);
  checkSensors();
  while(vals[1] > 200)
  {
    checkSensors();
  }
  rightServo.write(90);
}

// Turn right
void turnRight(){
  leftServo.write(180);
  rightServo.write(90);
  delay(600);
  checkSensors();
  while(vals[1] > 200)
  {
    checkSensors();
  }
  leftServo.write(90);
}

// Move forwrard
void goStraight(){
  leftServo.write(180);
  rightServo.write(0);
}

// Stop moving
void stop(){
  leftServo.write(90);
  rightServo.write(90);
}

// Logic for deciding the route for figure 8 for the robot
void decideRoute() {
  checkSensors();
  int leftSpeed; int rightSpeed; int direction;
  if (vals[0]>200 && vals[2]>200 && vals[1] < 200){  //go straight
    goStraight();
  }

  // take 8 steps for the figure 8 route
  else if (vals[0] < 200 && vals[1] < 200 && vals[2] < 200)  //intersection initiate turn
  {
  	direction = wallDetected();
  	if (direction == 0){
  	  // No wall detected to right, so turn right
  	  turnRight();
  	}
  	else if (direction == 1){
  	  // Wall detected to right AND in front, so turn left
  	  turnLeft();
  	}
  	else if (direction == 2){
  	  // Wall detected to right, but NOT in front, so move forward
  	  goStraight();
  	}
  	else {
  	  Serial.println("Sorry something really weird happened :^(")
  	  stop();
  	}
  }

  // Adjust the robot a bit if it's off the line
  else if(vals[2] < 200){   //right sensor on line now turn right
    turnRight();
  }
  else if(vals[0] < 200){  //left sensor on line now turn left
    turnLeft();
  }

  // Stop the robot if it's off any white lines
  else{ //stop
    stop();
  }
}
