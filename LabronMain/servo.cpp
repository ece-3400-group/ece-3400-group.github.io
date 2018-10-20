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
const int leftLineThresh = 200;
const int middleLineThresh = 200;
const int rightLineThresh = 200;


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
  if (debug && left != 90  || right !=90) {
    Serial.print( "Left Servo = [ " ); Serial.print( left ); Serial.println( " ]");
    Serial.print( "Rightt Servo = [ " ); Serial.print( right ); Serial.println( " ]");
  }
}

// Check the IR sensor data
void checkSensors(){
  // Read the IR sensor data
  vals[0] = analogRead(QRE1113_Pin0);  // Right sensor
  vals[1] = analogRead(QRE1113_Pin1);  // Middle Sensor
  vals[2] = analogRead(QRE1113_Pin2);  // Left Sensor
//  readServos();
//  Serial.print("right: ");Serial.println(vals[0]);
//  Serial.print("mid: ");Serial.println(vals[1]);
//  Serial.print("left: ");Serial.println(vals[2]);
//  Serial.println("");
}

// Turn left
void turnLeft(){
  leftServo.write(90);
  rightServo.write(0);
  delay(600);
  checkSensors();
  while(vals[1] > middleLineThresh)
  {
    checkSensors();
  }
  rightServo.write(90);
}

// Turn slightly left
void slightLeft(){
  leftServo.write(90);
  rightServo.write(0);
  delay(50);
  goStraight();
}

// Turn slightly right
void slightRight(){
  leftServo.write(180);
  rightServo.write(90);
  delay(50);
  goStraight();
}

// Turn right
void turnRight(){
  leftServo.write(180);
  rightServo.write(90);
  delay(500);
  checkSensors();
  while(vals[1] > middleLineThresh)
  {
    checkSensors();
  }
  leftServo.write(90);
}

//Turn around
void turnAround(){
  leftServo.write(180);
  rightServo.write(0);
  delay(400);
  checkSensors();
  while(vals[1] > 200){
    checkSensors();
  }
  stop();
}

// Move forwrard
void goStraight(){
  leftServo.write(100);
  rightServo.write(80);
}

// Stop moving
void stop(){
  leftServo.write(90);
  rightServo.write(90);
}

// Logic for deciding the route for the robot
void decideRoute() {
  checkSensors();
  Serial.println(vals[1]);  // Taking out all the print statements makes LABron act weird im leaving one in -R
  int leftSpeed; int rightSpeed; int direction;
  if (vals[0]> leftLineThresh && vals[2]>rightLineThresh && vals[1] < middleLineThresh){  //go straight
    //Serial.println("If statement");
    goStraight();
  }

  // take 8 steps for the figure 8 route
  else if (vals[0] < leftLineThresh && vals[1] < middleLineThresh && vals[2] < rightLineThresh)  //intersection initiate turn
  {
    stop();
  	direction = wallDetected();
  	if (direction == 0){
  	  // No wall detected to right, so turn right
  	  turnRight();
      digitalWrite(RightWallPin, LOW);
      digitalWrite(ForwardWallPin, LOW);
    }
    else if (direction == 1){
      // Wall detected to right AND in front, so turn left
      digitalWrite(RightWallPin, HIGH);
      digitalWrite(ForwardWallPin, HIGH);
      turnLeft();
     digitalWrite(RightWallPin, LOW);
      digitalWrite(ForwardWallPin, LOW);
     //Serial.println("forward Wall detected");
    }
    else if (direction == 2){
      // Wall detected to right, but NOT in front, so move forward -- the while loop keeps LABron moving off the intersection
      digitalWrite(RightWallPin, HIGH);
      digitalWrite(ForwardWallPin, LOW);
      goStraight();
      while(vals[0] < rightLineThresh || vals[2] < leftLineThresh){
        checkSensors();
      }
  	  stop();
  	}
  	else {
  	  //Serial.println("Sorry something really weird happened :^(");
  	  stop();
  	}
  }

  // Adjust the robot a bit if it's off the line
  else if(vals[2] < leftLineThresh){   //left sensor on line now adjust left
    //Serial.println("############################");
    slightLeft();
  }
  else if(vals[0] < rightLineThresh){  //right sensor on line now adjust right
    slightRight();
  }

  // Stop the robot if it's off any white lines
  else{ //stop
    stop();
  }
}
