//Code for line following
#include <Servo.h>

// Initialize Servos
Servo rightServo;
Servo leftServo;


// assign pins to the Servos
const int RIGHT_PIN = 5; // Right servo control pin
const int LEFT_PIN  = 6; // Left servo control pin
const int LED = 7;       // External LED control pin

//Outputs via the serial terminal – Lower numbers mean more reflected
const int QRE1113_Pin0 = 0; //connected to analog 0
const int QRE1113_Pin1 = 1; //connected to analog 1
const int QRE1113_Pin2 = 2; //connected to analog 2
int vals [3];         //container for sensor output
int returned [2];
int routeNums [2];    //container for route direction
int leftSpeed = 0;
int rightSpeed = 0;
bool fig8 = true;
int step = 0;


// Inver the number for the servo to make the 0 - 90 range logic intuitive
int invert(int number){
  return abs(number-180);
}

void straightenServo(){
  // Finish turn by setting both servos to equal (0) speed
  leftServo.write(90);
  rightServo.write(90);
}

void slightLeft(){
  leftServo.write(60);
  rightServo.write(60);
  delay(50);
  straightenServo();
}


void slightRight(){
  leftServo.write(120);
  rightServo.write(120);
  delay(50);
  straightenServo();
}

void adjustRoute(){
  checkSensors(vals);
  if (vals[0]<vals[1]&&vals[0]<vals[2]){//left sensor is least
    slightLeft();
  }
  else if ((vals[2]<vals[0])&&(vals[2]<vals[1])){//right sensor is least
    slightRight();
  }
  else{ //middle is least, LeBron is on track
    return;
  }
}

void turnLeft(){
  leftServo.write(60);
  rightServo.write(60);
  delay(600);
  straightenServo();
}


void turnRight(){
  leftServo.write(120);
  rightServo.write(120);
  delay(600);
  straightenServo();
  leftServo.write(90);
  rightServo.write(90);
}


void moveToIntersection(){
  // move forward a little bit so that LeBron can turn
  leftServo.write(180);
  rightServo.write(0);
  delay(350);
  leftServo.write(90);
  rightServo.write(90);
}


void setup(){
Serial.begin(9600);
pinMode(A0,INPUT);
pinMode(A1,INPUT);
pinMode(A2,INPUT);

// Attach the Servos to their corresponding pins
rightServo.attach(RIGHT_PIN);
leftServo.attach(LEFT_PIN);

}

void checkSensors(int valsLocal []){
  
int QRE_Value0 = analogRead(QRE1113_Pin0);
valsLocal[0] = analogRead(QRE1113_Pin0);
int QRE_Value1 = analogRead(QRE1113_Pin1);
valsLocal[1] = analogRead(QRE1113_Pin1);
int QRE_Value2 = analogRead(QRE1113_Pin2);
valsLocal[2] = analogRead(QRE1113_Pin2);

//Serial.print(String(valsLocal[0]) + ", " + String(valsLocal[1]));
//Serial.println(", " + String(valsLocal[2]));
}

void decideRoute(int sensorVals []){

  if (sensorVals[0]>200 && sensorVals[2]>200 && sensorVals[1] < 200){  //go straight
    //Serial.println("STRAIGHT");
    leftSpeed = 180;
    rightSpeed = 0;
  }
  else if (sensorVals[0] < 200 && sensorVals[1] < 200 && sensorVals[2] < 200)  //intersection initiate turn
  {
    moveToIntersection();
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
    step = step % 8;
  }
  else if(sensorVals[2] < 200){   //right sensor on line now turn right
    leftSpeed = 180;
    rightSpeed = 70;
  }
  else if(sensorVals[0] < 200){  //left sensor on line now turn left
    leftSpeed = 110;
    rightSpeed = 0;
  }
  else{ //stop
    leftSpeed=90;
    rightSpeed=90;
  }

  returned [0]= leftSpeed; 
  returned [1] = rightSpeed;
//  Serial.print("Left speed: ");
//  Serial.println(leftSpeed);
//  Serial.print("Right speed: ");
//  Serial.println(rightSpeed);
//  Serial.println("");
}

void servoSpeed(){
  leftServo.write(returned[0]); // left Servo speed
  rightServo.write(returned[1]); // right Servo speed
}



void loop(){
checkSensors(vals);
decideRoute(vals);
servoSpeed();
delay(20);        // wait 20 ms
adjustRoute();    // adjust route to maximize chance of staying in line
delay(10);        // wait 10 ms

}
