#include "FFTs.h"
#include <FFT.h> // include the library

const int binWhistle = 6;       // 660Hz BIN
const int binIRHat   = 44;      // 6.08kHz BIN
const int binIRDecoy = 122;     // 18kHz BIN
const int audioThreshold = 100;
const int hatThreshold = 100;
const int decoyThreshold = 60;

void setupFFT(){
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x44; // use adc4
  DIDR0 = 0x01; // turn off the digital input for adc0
}

byte readFFT(int adcPinNum = ADC0_FFT) {
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

  byte result = isFFTPeak();
  return result;
}

byte isFFTPeak() {
  byte result = 0b0000;
  if (fft_log_out[binWhistle]>audioThreshold) {
    result |= 0b0001;
  }
  if (fft_log_out[binIRHat]>hatThreshold) {
    result |= 0b0010;
  }
  if (fft_log_out[binIRDecoy]>decoyThreshold) {
    result |= 0b0100;
  }
  return result;
}

void displayLedFFT( byte fftResult, int audioPin, int hatPin, int decoyPin ) {
  if (fftResult & 0b01 == 1) digitalWrite(audioPin, HIGH);
  else digitalWrite(audioPin, LOW);

  if (fftResult & 0b010 == 1) digitalWrite(hatPin, HIGH);
  else digitalWrite(hatPin, LOW);

  if (fftResult & 0b0100 == 1) digitalWrite(decoyPin, HIGH);
  else digitalWrite(decoyPin, LOW);
}

void debugFFT(){
    int maxbin = 2;
    for (int i=2; i<FFT_N/2;i++){   // start at a bin above 0 so that DC IR light is not compared
      if (fft_log_out[i]>fft_log_out[maxbin]){
        maxbin = i;
      }
    }
    for (int i=binWhistle-2;i<binWhistle+2;i++){    // search in range of plus and minus 2
      int threshold = 100;
      if (fft_log_out[i]>threshold){
        Serial.println("Whistle detected!");
        Serial.println(fft_log_out[i]);
      }
    }

    for (int i=binIRHat-2;i<binIRHat+2;i++){       // search in range of plus and minus 2
      int threshold = 100;
      if (fft_log_out[i]>threshold){
        Serial.println("IR Hat detected!");
        Serial.println(fft_log_out[i]);
      }
    }

    for (int i=binIRDecoy-2;i<binIRDecoy+2;i++){    // search in range of plus and minus 2
      int threshold = 60;
      if (fft_log_out[i]>threshold){
        Serial.println("IR Decoy detected!");
        Serial.println(fft_log_out[i]);
      }
    }

  }
