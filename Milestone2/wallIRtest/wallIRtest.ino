// Code for IR sensor test


float averageDistanceIRReading(int delayTime, int IRpin,int n){
  // Average n measurements from specified IRPin separated by delayTime amount of time
    int readings [n] = {};
    for (int i=0; i<n; i++){
      readings[i] = analogRead(IRpin);
      delay(delayTime);      
  }
   
    float average = 0.0; 
    for (int i=0; i<n; i++){
      //Serial.print("Readings " ); Serial.println(readings[i]);
      average += readings[i];
    }
    average = average/((float)n); // avoid integer division issues
    //Serial.print("Average "); 
    Serial.println(average);
    return average;
}

bool wallDetected(int wallThreshold){
  // Using an average measurement for a period of timesteps
  // decide if a wall is there.

  int delayTime = 30; 
  int IRPin = A3; 
  int n = 5;

  float average = averageDistanceIRReading(delayTime,IRPin,n); // take an average to be less sensitive to noise

  if (average < wallThreshold){
    return true; 
  }
  else{
    return false; 
  }
  return false; 

}
  

void setup(){
  Serial.begin(9600);           // Start Serial monitor for debugging
  pinMode(A3,INPUT);  // IR sensor 1
}

void loop(){
  bool wallDetect = wallDetected(250); //parameter is threshold
  if (wallDetect){
    //Serial.println("Wall detected");
  }
  else{
   // Serial.println ("No wall detected");
  }
 // Serial.println (analogRead(A3));
 // delay(100);
}
