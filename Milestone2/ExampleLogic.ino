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
      average += readings[i];
    }
    average = average/((float)n); // avoid integer division issues
    return average;
}

int wallDetected(int wallThreshold){
  // Using an average measurement for a period of timesteps
  // decide if a wall is there.
  
  int delayTime = 30; 
  int forwardWall = A3; 
  int rightWall = A4;
  int n = 5;

  int averageForward = averageDistanceIRReading(delayTime,forawardWall,n); // take an average to be less sensitive to noise
  int averageRight = averageDistanceIRReading(delayTime,rightWall,n);
  
  if (averageRight < wallThreshold){
    return 0;     // Turn right (R sensor doesn't detect wall)
  }
  else{
    if(forwardSensor < wallThreshold){
      return 1;   // Continue moving forward (only R sensor detects wall)
    }
    return 2;     // Turn left (both F and R sensors detect wall)
}
  

void setup(){
  Serial.begin(9600);           // Start Serial monitor for debugging
  pinMode(A3,INPUT);            // Forward Sensor
  pinMode(A4,INPUT);            // Right Sensor
}

void loop(){
  bool wallDetect = wallDetected(300); //using 300 as threshold for wall detection
  if (wallDetect == 0){
    // turn right
  }
  }
  else if (wall Detect == 1){
    // move forward
  }
  else{
    // turn left
  }
}
