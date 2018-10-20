#include "FFTs.h"
#include <FFT.h> // include the library
#include "servo.h"

const int MASTER_FFT_PIN = A5;
const int audioPin = 8;
const int hatPin = 9;
const int decoyPin = 10;

const int binWhistle = 6;       // 660Hz BIN
const int binIRHat   = 44;      // 6.08kHz BIN
const int binIRDecoy = 122;     // 18kHz BIN
const int audioThreshold = 100;
const int hatThreshold = 50;
const int decoyThreshold = 100;
byte result; byte fftBefore;

void setupDebugFFT(){
  pinMode(audioPin, OUTPUT);
  pinMode(hatPin, OUTPUT);
  pinMode(decoyPin, OUTPUT);
}

void setupFFT(){
  //TIMSK0 = 0; // turn off timer0 for lower jitter
  /* Adding these two lines wouldn't make the fft work
  //ADCSRA = 0xe5; // set the adc to free running mode
  //ADMUX = 0x45; // use adc4
  */
 // DIDR0 = 0x05; // turn off the digital input for adc0
  pinMode(MASTER_FFT_PIN, INPUT);
}

byte readFFT(int adcPinNum = ADC5_FFT) {
  ADMUX = adcPinNum;
  int pastADCReading = ADCSRA;
  ADCSRA = 0xe5;
  cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
    while(!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fft_input[i] = k; // put real data into even bins
    fft_input[i+1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();
  ADCSRA = pastADCReading;

//  for (byte i = 0 ; i < FFT_N/2 ; i++) { 
//     Serial.print(fft_log_out[i]); // send out the data
//     Serial.print("\t");
//  }
//   //Serial.println(" "); 

  byte result = isFFTPeak();
  return result;
}

byte waitForStart() {
  Serial.println("Waiting for start"); // DO NOT DELETE THIS COMMENT
  stop();
  byte fftReading = readFFT();
  if (fftReading & AUDIO_MASK) {
    Serial.println("------------------------------------------------------ started------");
    return 0b0000;
  }
  return 0b0001;
}

byte isFFTPeak() {
  fftBefore = result++ % 3;
  result = 0b0000;
  for (int i=binWhistle-1;i<binWhistle+1;i++){    // search in range of plus and minus 2
      if (fft_log_out[i]>audioThreshold) {
        result |= AUDIO_MASK;
        Serial.println("DETECTEDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
      }
  }

    for (int i=binIRHat-2;i<binIRHat+2;i++){       // search in range of plus and minus 2
      if (fft_log_out[i]>hatThreshold) result |= IRHAT_MASK;
    }

    for (int i=binIRDecoy-2;i<binIRDecoy+2;i++){    // search in range of plus and minus 2
      if (fft_log_out[i]>decoyThreshold)result |= DECOY_MASK;
    }
  return result;
}

void displayLedFFT( byte fftResult, int audioPin, int hatPin, int decoyPin ) {
  if ((fftResult & AUDIO_MASK) && (fftBefore & AUDIO_MASK)) {
    //Serial.println("TURNING ON LED****************************************");
    digitalWrite(audioPin, HIGH);
  }
  else digitalWrite(audioPin, LOW);

  if (fftResult & IRHAT_MASK) digitalWrite(hatPin, HIGH);
  else digitalWrite(hatPin, LOW);

  if (fftResult & DECOY_MASK) digitalWrite(decoyPin, HIGH);
  else digitalWrite(decoyPin, LOW);
}

void debugFFT(){
    int maxbin = 2;
    for (int i=2; i<FFT_N/2;i++){   // start at a bin above 0 so that DC IR light is not compared
      if (fft_log_out[i]>fft_log_out[maxbin]){
        maxbin = i;
      }
    }
    for (int i=binWhistle-1;i<binWhistle+1;i++){    // search in range of plus and minus 2
      if (fft_log_out[i]>audioThreshold){
        Serial.print("Whistle detected!");
        Serial.println(fft_log_out[i]);
      }
    }

    for (int i=binIRHat-2;i<binIRHat+2;i++){       // search in range of plus and minus 2
      if (fft_log_out[i]>hatThreshold){
        Serial.print("IR Hat detected!");
        Serial.println(fft_log_out[i]);
        Serial.print("turning around");        
      }
    }

    for (int i=binIRDecoy-2;i<binIRDecoy+2;i++){    // search in range of plus and minus 2
      if (fft_log_out[i]>decoyThreshold){
        Serial.print("IR Decoy detected!");
        //Serial.println(fft_log_out[i]);
        
      }
    }

  }
