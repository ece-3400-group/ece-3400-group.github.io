#include <Servo.h>

// Initialize Servos
Servo rightServo;
Servo leftServo;

// assign pins to the Servos
const int RIGHT_PIN = 5; // Right servo control pin
const int LEFT_PIN  = 6; // Left servo control pin
const int LED = 7;       // External LED control pin

// Inver the number for the servo to make the 0 - 90 range logic intuitive
int invert(int number){
  return abs(number-180);
}

// Turn the robot left
void turnLeft(){
  int valLeft = 45;  // go slowly
  int valRight = 0;  // go quickly
  leftServo.write (valLeft);
  rightServo.write(valRight);
  delay(500);        // wait 0.5 seconds
}

// Turn the robot right
void turnRight(){
  int valLeft = 0;    // go quickly
  int valRight = 45;  // go slowly
  leftServo.write (valLeft);
  rightServo.write(valRight);
  delay(500);         // wait 0.5 seconds
}

// Go straight at a certain pace
void straight(int pace){
  int valLeft = pace;          // write a pace value
  int valRight = invert(pace); // invert the pace since the wheels opposite
  leftServo.write (valLeft);
  rightServo.write(valRight);
  delay(1000);                 // wait 1 second
}

// Make a (smooth) figure 8 with the robot
void figEight(int pace){
  // Make the one half of the 8
  straight(pace);  
  turnLeft();        
  straight(pace/2);
  turnLeft();
  straight(pace/2);
  turnLeft();
  // Make the second half of the 8
  turnRight();
  straight(pace/2);
  turnRight();
  straight(pace/2);
  turnRight();
}

void setup() {
  // Attach the Servos to their corresponding pins
  rightServo.attach(RIGHT_PIN);
  leftServo.attach(LEFT_PIN);

  // Turn on indicator LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
}

void loop() {
  figEight(50); // Draw a figure 8 at pace 50
  delay(1000);  // Wait 1 second
}


