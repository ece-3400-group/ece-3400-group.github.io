#include "servo.h"
#include <Arduino.h>

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

// Write a speed to the servos
void servoSpeed(int leftSpeed, int rightSpeed){
  leftServo.write(leftSpeed);   // left Servo speed
  rightServo.write(rightSpeed); // right Servo speed
}

// Stop servo wheels
void straightenServo(){
  // Finish turn by setting both servos to equal (0) speed
  leftServo.write(90);
  rightServo.write(90);
}

// Turn slightly left
void slightLeft(){
  leftServo.write(60);
  rightServo.write(60);
  delay(50);
  straightenServo();
}

// Turn slightly right
void slightRight(){
  leftServo.write(120);
  rightServo.write(120);
  delay(50);
  straightenServo();
}

// Turn left smoothly
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

// Turn right smoothly

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

// Move to the intersection for right angle turns
void moveToIntersection(){
  // move forward a little bit so that LeBron can turn
  leftServo.write(180);
  rightServo.write(0);
  delay(350);
  leftServo.write(90);
  rightServo.write(90);
}

void goStraight(){
  leftServo.write(180);
  rightServo.write(0);
  checkSensors();
}

// Logic for deciding the route for figure 8 for the robot
void decideRoute() {
  int leftSpeed; int rightSpeed; byte step;
  if (vals[0]>200 && vals[2]>200 && vals[1] < 200){  //go straight
    leftSpeed = 180; // max speed
    rightSpeed = 0;  // max speed
  }

  // take 8 steps for the figure 8 route
  else if (vals[0] < 200 && vals[1] < 200 && vals[2] < 200)  //intersection initiate turn
  {
    if (step < 2){
      step++;
      turnLeft();
    }
    else if (step <= 5){
      step++;
      turnRight();
    }
    else {
      step++;
      turnLeft();
    }
    step  = step % 8; // repeat for figure 8
  }

  // Adjust the robot a bit if it's off the line
  else if(vals[2] < 200){   //right sensor on line now turn right
    leftSpeed = 180;
    rightSpeed = 90;
  }
  else if(vals[0] < 200){  //left sensor on line now turn left
    leftSpeed = 90;
    rightSpeed = 0;
  }

  // Stop the robot if it's off any white lines
  else{ //stop
    leftSpeed=90;
    rightSpeed=90;
  }

  // Set the speeds for the servos
  servoSpeed(leftSpeed, rightSpeed);
//  Serial.print("Left speed: ");
//  Serial.println(leftSpeed);
//  Serial.print("Right speed: ");
//  Serial.println(rightSpeed);
//  Serial.println("");
}


// Adjust the routes of the servos
void adjustRoute(){
  // Read the 3 sensor values
  checkSensors();

  // Nudge robot path a little left
  if (vals[0]<vals[1]&&vals[0]<vals[2]){         //left sensor is least
    slightLeft();
  }
  // Nudge robot path a little right
  else if ((vals[2]<vals[0])&&(vals[2]<vals[1])){//right sensor is least
    slightRight();
  }
  else{ //middle is least (most white), LeBron is on track
    return;
  }
}
