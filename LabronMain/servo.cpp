#include "servo.h"
#include <Arduino.h>
#include "sensors.h"

/*==================== [ Initialize Servos ] ================in===============*/
Servo rightServo;
Servo leftServo;

int vals[3];         // container for sensor output

void setupLineSensors() {
  pinMode(QRE1113_PIN0,INPUT);  // IR sensor 1
  pinMode(QRE1113_PIN1,INPUT);  // IR sensor 2
  pinMode(QRE1113_PIN2,INPUT);  // IR sensor 3
}

void setupServos() {
    // Attach the Servos to their corresponding pins
  rightServo.attach(RIGHTSERVO_PIN);
  leftServo.attach(LEFTSERVO_PIN);
}

void readServos(byte debug=1) {
  int left = leftServo.read();
  int right = rightServo.read();
  if (debug) {
    Serial.print(F("Left Servo = [ " )); Serial.print( left ); Serial.println( F(" ]"));
    Serial.print(F("Rightt Servo = [ ") ); Serial.print( right ); Serial.println( F(" ]"));
  }
}

// Check the IR sensor data
void checkSensors(){
  // Read the IR sensor data
  vals[0] = analogRead(QRE1113_PIN0);  // Right sensor
  vals[1] = analogRead(QRE1113_PIN1);  // Middle Sensor
  vals[2] = analogRead(QRE1113_PIN2);  // Left Sensor
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
  while(vals[1] > LINE_THRESHOLD)
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
  while(vals[1] > LINE_THRESHOLD)
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
  while(vals[1] > LINE_THRESHOLD){
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
byte decideRoute() {
  checkSensors();
  Serial.println(vals[1]);  // Taking out all the print statements makes LABron act weird im leaving one in -R
  int leftSpeed; int rightSpeed; byte direction = 0b00000000;
  if (vals[0]>LINE_THRESHOLD && vals[2]>LINE_THRESHOLD && vals[1] < LINE_THRESHOLD){  //go straight
    //Serial.println("NOTHING");
    goStraight();
    return 0;          // No intersection reached, so return 0 (no radio information necessary)
  }

  else if (vals[0] < LINE_THRESHOLD && vals[1] < LINE_THRESHOLD && vals[2] < LINE_THRESHOLD)  //intersection initiate turn
  {
    stop();
  	direction = wallDetected();
  	if (direction == 0b10000000 || direction == 0b00000000){
  	  // No wall detected to right, so turn right
      direction = direction | 0b000001000;
  	  turnRight();
      digitalWrite(RIGHTWALL_PIN, LOW);
      digitalWrite(FORWARDWALL_PIN, LOW);
     // Serial.println("TURNING RIGHT");
  	}
  	else if (direction == 0b11000000){
  	  // Wall detected to right AND in front, so turn left
      digitalWrite(RIGHTWALL_PIN, HIGH);
      digitalWrite(FORWARDWALL_PIN, HIGH);
      direction = direction | 0b00000010;
  	  turnLeft();
     //Serial.println("TURN LEFT");
      digitalWrite(RIGHTWALL_PIN, LOW);
      digitalWrite(FORWARDWALL_PIN, LOW);
  	}
  	else if (direction == 0b01000000){
  	  // Wall detected to right, but NOT in front, so move forward
  	  digitalWrite(RIGHTWALL_PIN, HIGH);
      digitalWrite(FORWARDWALL_PIN, LOW);
      direction = direction | 0b00001000;
      goStraight();
     // Serial.println("MOVING FORWARD");
      while(vals[0] < LINE_THRESHOLD || vals[2] < LINE_THRESHOLD){
        checkSensors();
      }
  	  stop();
  	}
  	else {
  	  Serial.println(F("Sorry something really weird happened :^("));
  	  stop();
  	}
   return direction;   
  }

  // Adjust the robot a bit if it's off the line
  else if(vals[2] < LINE_THRESHOLD){   //left sensor on line now adjust left
    //Serial.println("ADJUSTING LEFT");
    slightLeft();          // No intersection reached, so return 0 (no radio information necessary)
    return 0;
  }
  else if(vals[0] < LINE_THRESHOLD){  //right sensor on line now adjust right
    //Serial.println("ADJUSTING RIGHT");
    slightRight();          // No intersection reached, so return 0 (no radio information necessary)
    return 0;
  }

  // Stop the robot if it's off any white lines
  else{ //stop
    //Serial.println("STOPPING");
    stop();                // No intersection reached, so return 0 (no radio information necessary)
    return 0;
  }
}
