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
  Serial.println("Left");
  leftServo.write(90);
  rightServo.write(0);
  delay(300);
  checkSensors();
  while(vals[1] > 800)
  {
    checkSensors();
  }
  rightServo.write(90);
}

// Turn slightly left
void slightLeft(){
  Serial.println("Slight Left");
  leftServo.write(60);
  rightServo.write(60);
  delay(15);
  goStraight();
  delay(5);
  stop();
}

// Turn slightly right
void slightRight(){
  Serial.println("Slight Right");
  leftServo.write(120);
  rightServo.write(120);
  delay(15);
  goStraight();
  delay(5);
  stop();
}

// Turn right
void turnRight(){
  Serial.println("RIGHT");
  leftServo.write(180);
  rightServo.write(90);
  delay(300);
  checkSensors();
  while(vals[1] > 800)
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
  while(vals[1] > 800){
    checkSensors();
  }
  stop();
}

// Move forwrard
void goStraight(){
  Serial.println("Straight");
  leftServo.write(100);
  rightServo.write(80);
  //delay(100);
  //stop();
}

// Stop moving
void stop(){
  leftServo.write(90);
  rightServo.write(90);
}

// Logic for deciding the route for the robot
void decideRoute() {
  checkSensors();
  int leftSpeed; int rightSpeed; int direction;
  if (vals[0]>800 && vals[2]>800 && vals[1] < 800){  //go straight
    Serial.println("If statement");
    goStraight();
  }

  // take 8 steps for the figure 8 route
  else if (vals[0] < 800 && vals[1] < 800 && vals[2] < 800)  //intersection initiate turn
  {
    stop();
    Serial.println("Else if statement");
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
     Serial.println("forward Wall detected");
  	}
  	else if (direction == 2){
  	  // Wall detected to right, but NOT in front, so move forward
  	  digitalWrite(RightWallPin, HIGH);
      digitalWrite(ForwardWallPin, LOW);
  	  goStraight();
     
     Serial.println("right");
  	}
  	else {
  	  Serial.println("Sorry something really weird happened :^(");
  	  stop();
  	}
  }

  // Adjust the robot a bit if it's off the line
  else if(vals[2] < 800){   //right sensor on line now turn right
    slightRight();
  }
  else if(vals[0] < 800){  //left sensor on line now turn leftdigitalWrite(RightWallPin, LOW);

    slightLeft();
  }

  // Stop the robot if it's off any white lines
  else{ //stop
    stop();
  }
}
