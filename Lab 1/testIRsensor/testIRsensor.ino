//Code for line following
#include "servo_control.h"

/*==================== [ Initialize Servos ] ===============================*/
Servo rightServo;
Servo leftServo;

/*==================== [ Assign Pins ] ===============================*/
const int RIGHT_PIN = 5; // Right servo control pin
const int LEFT_PIN  = 6; // Left servo control pin
const int LED = 7;       // External LED control pin

/*======================= [ Global Variables ] ====================*/
int servoSpeeds [2];  // container for servo speeds
int routeNums [2];    // container for route direction
int leftSpeed = 0;    
int rightSpeed = 0;
bool fig8 = true;     // bool, do the figure 8
int step = 0;

/*==========================[ Functions ]===========================*/
// To be put in another file later
// Invert the number for the servo to make the 0 - 90 range logic intuitive
int invert(int number){
  return abs(number-180);
}

// Logic for deciding the route for figure 8 for the robot
void decideRoute(int sensorVals[]){
  if (sensorVals[0]>200 && sensorVals[2]>200 && sensorVals[1] < 200){  //go straight
    leftSpeed = 180; // max speed
    rightSpeed = 0;  // max speed
  }

  // take 8 steps for the figure 8 route
  else if (sensorVals[0] < 200 && sensorVals[1] < 200 && sensorVals[2] < 200)  //intersection initiate turn
  {
    if (step < 2){
      step++;
      turnLeft(leftServo, rightServo);
    }
    else if (step <= 5){
      step++;
      turnRight(leftServo, rightServo);
    }
    else {
      step++;
      turnLeft(leftServo, rightServo);
    }
    step  = step % 8; // repeat for figure 8
  }

  // Adjust the robot a bit if it's off the line
  else if(sensorVals[2] < 200){   //right sensor on line now turn right
    leftSpeed = 180;
    rightSpeed = 90;
  }
  else if(sensorVals[0] < 200){  //left sensor on line now turn left
    leftSpeed = 90;
    rightSpeed = 0;
  }

  // Stop the robot if it's off any white lines
  else{ //stop
    leftSpeed=90;
    rightSpeed=90;
  }

  // Set the speeds for the servos
  servoSpeeds [0]= leftSpeed; 
  servoSpeeds [1] = rightSpeed;
//  Serial.print("Left speed: ");
//  Serial.println(leftSpeed);
//  Serial.print("Right speed: ");
//  Serial.println(rightSpeed);
//  Serial.println("");
}

// Adjust the routes of the servos
void adjustRoute(Servo leftServo, Servo rightServo, int vals[]){
  // Read the 3 sensor values
  checkSensors(vals);

  // Nudge robot path a little left
  if (vals[0]<vals[1]&&vals[0]<vals[2]){         //left sensor is least
    slightLeft(leftServo, rightServo);
  }
  // Nudge robot path a little right
  else if ((vals[2]<vals[0])&&(vals[2]<vals[1])){//right sensor is least
    slightRight(leftServo, rightServo);
  }
  else{ //middle is least (most white), LeBron is on track
    return;
  }
}

void setup(){
  Serial.begin(9600);           // Start Serial monitor for debugging
  pinMode(QRE1113_Pin0,INPUT);  // IR sensor 1
  pinMode(QRE1113_Pin1,INPUT);  // IR sensor 2
  pinMode(QRE1113_Pin2,INPUT);  // IR sensor 3

  // Attach the Servos to their corresponding pins
  rightServo.attach(RIGHT_PIN);
  leftServo.attach(LEFT_PIN);
}

void loop(){
  checkSensors(vals);  // Check all sensors
  decideRoute(vals);   // Make the route
  servoSpeed(leftServo, rightServo, servoSpeeds[0], servoSpeeds[1]);
  delay(20);        // wait 20 ms
  //adjustRoute(leftServo, rightServo, vals);    // adjust route to maximize chance of staying in line
  //delay(10);        // wait 10 ms
}
