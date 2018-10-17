// Imports for facilitating timer implementation
#include <TimerOne.h>

 
volatile int result2; 
volatile int result3; 
volatile long globalTime; // Time (ms) since program setup is complete for process reference

void testISR2(){
  // A test Interrupt Service Routine 
  result2 = digitalRead(2);
  Serial.println ("ISR Pin " + 2); 
}


void testISR3(){
  // A test Interrupt Service Routine 
  result3 = digitalRead(3);
  Serial.println ("ISR Pin " + 3);  
}



void ISRTimer1(){  
   // Timer 0 interrupt commands here
   Serial.println("Timer 1 boop");
}

void ISRTimer3(){  
   // Timer 1 interrupt commands here
      Serial.println("Timer 3 beep");
}

void setTimer1(int period){
  if (period < 0){
    return; 
  }

//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}


void setTimer2(int period){
  if (period < 0){
    return; 
  }

//set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
}

void setup() {
  
  const int EXTERNAL_PIN_2 = 2;
  const int EXTERNAL_PIN_3 = 3;
  // put your setup code here, to run once:
  noInterrupts(); 

  //*********************************************************************
  // DIGITAL PIN INTERRUPT SETUP
  // Pins 2, 3 are external interrupt pins (independent interrupt vector)
  //*********************************************************************
  Serial.begin(9600);
  pinMode(EXTERNAL_PIN_2,OUTPUT); // to write resistor (?)
  pinMode(EXTERNAL_PIN_3,OUTPUT);
  digitalWrite(EXTERNAL_PIN_2,HIGH); // enable internal pullup resistor
  digitalWrite(EXTERNAL_PIN_3,HIGH); // enable internal pullup resistor

  pinMode(EXTERNAL_PIN_2, INPUT); 
  pinMode(EXTERNAL_PIN_3, INPUT); 
  attachInterrupt (digitalPinToInterrupt (EXTERNAL_PIN_2), testISR2, LOW);  // attach interrupt handler
  attachInterrupt (digitalPinToInterrupt (EXTERNAL_PIN_3), testISR3, CHANGE);  // attach interrupt handler

  //*********************************************************************
  // TIMER INTERRUPT SETUP
  // Arduino PWM pins are 9 and 10. These can be initialized with TimerX.pwm(pin,1024*dutyCycle);
  //*********************************************************************
  
  Timer1.initialize(5000000); // initialize timer1, and set period to 5 s (arg in microseconds)
  // Timer1.pwm(9, 512); // setup pwm on pin 9, 50% duty cycle
  Timer1.attachInterrupt(ISRTimer1); // attaches Timer1ISR as a timer overflow interrupt

  // timer frequency = 16MHZ / prescaler 
  // compare match register = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1
  // interrupt frequency (Hz) = (Arduino clock speed 16,000,000Hz) / (prescaler * (compare match register + 1))
  
  // Set timers with helper function. Pass negative argument to not set timer. 
  //  setTimer(-1);
  //  setTimer(-1);
  //  setTimer(-1); 
  

  interrupts(); 
  globalTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  globalTime = millis(); // 
  if (globalTime % 2000 == 0){
    Serial.println("Loop timer bebop"); 
    Serial.print(globalTime/2000);
    Serial.println(" seconds");
  }
}
