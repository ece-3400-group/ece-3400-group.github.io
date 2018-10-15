/*
it takes in data on ADC0 (Analog0) and processes them
with the fft. the data is sent out over the serial
port at 18kHz.
*/
#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft
//#include <FFT.h> // include the library

int adc_5 = 0x45;

void setup() {
  Serial.begin(9600); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter
  //ADCSRA = 0xe5; // set the adc to free running mode
  //ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

  pinMode(A4,INPUT);  // IR sensor 1
}

// Code for IR sensor test
int IRPin = A4; 

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
    //Serial.println(average);
    return average;
}

float wallDetected(){
  // Using an average measurement for a period of timesteps
  // decide if a wall is there.

  int delayTime = 30; 
  int n = 5;

  float average = averageDistanceIRReading(delayTime,IRPin,n); // take an average to be less sensitive to noise
  return average;
  

}

void loop(){
  float wallDetect = wallDetected(); //parameter is threshold
  if (wallDetect > 400){
    Serial.print("Wall detected  ");
    Serial.println (wallDetect);
  }
  else{
    Serial.print("No wall detected  ");
    Serial.println (wallDetect);
  }
  //readfft(adc_5);
  //Serial.print (analogRead(A4));
  delay(1000);
}
