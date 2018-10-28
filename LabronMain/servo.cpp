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
    byte walldir = wallDetected();
    // bits [7-4] are wall directions based on sensors, bits [3-0] are the directions we choose to go
    // bit[3] forward, bit[2]  right, bit[1] turn aroudn, bit[0] left, all 0 do nothing
    if (walldir == FRONT || walldir == NOWALL || walldir == (FRONT | LEFT)){
      // No wall detected to right, so turn right
            Serial.println("No Wall");
      walldir = walldir | 0b000000100;
      turnRight();
      digitalWrite(RIGHTWALL_PIN, LOW);
      digitalWrite(FORWARDWALL_PIN, LOW);

    }
    else if ((walldir & FRONT) && (walldir & RIGHT) && (walldir & LEFT)){
      // Wall detected to right, front,and left so turn around
      // walldir is 11010000
      digitalWrite(RIGHTWALL_PIN, HIGH);
      digitalWrite(FORWARDWALL_PIN, HIGH);
      // digitalWrite(LEFTWALL_PIN, HIGH); // not sure if this LED exists
      walldir = walldir | 0b00000010; // bit 1 corresponds to flipping direction
      //turnAround();
      stop();
     Serial.println("Wall to FRONT and RIGHT and LEFT");
      digitalWrite(RIGHTWALL_PIN, LOW);
      digitalWrite(FORWARDWALL_PIN, LOW);
    }
    else if ((walldir & FRONT) && (walldir & RIGHT)){
      // Wall detected to right AND in front, so turn left
      digitalWrite(RIGHTWALL_PIN, HIGH);
      digitalWrite(FORWARDWALL_PIN, HIGH);
      walldir = walldir | 0b00000001;
      turnLeft();
     Serial.println("Wall to RIGHT and FRONT");
      digitalWrite(RIGHTWALL_PIN, LOW);
      digitalWrite(FORWARDWALL_PIN, LOW);
    }
    else if (walldir & RIGHT){
      // Wall detected to right, but NOT in front, so move forward
      digitalWrite(RIGHTWALL_PIN, HIGH);
      digitalWrite(FORWARDWALL_PIN, LOW);
      walldir = walldir | 0b00001000;
      goStraight();
      Serial.println("Wall to RIGHT");
      while(vals[0] < LINE_THRESHOLD || vals[2] < LINE_THRESHOLD){
        checkSensors();
      }
      stop();
    }
    else {
      Serial.println(F("Sorry something really weird happened :^("));
      stop();
    }
   return walldir;
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
