/*
fft_adc_serial.pde
guest openmusiclabs.com 7.7.14
example sketch for testing the fft library.
it takes in data on ADC0 (Analog0) and processes them
with the fft. the data is sent out over the serial
port at 115.2kb.
*/

#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library

void setup() {
  Serial.begin(9600); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x45; // use adc4
  DIDR0 = 0x01; // turn off the digital input for adc0
  pinMode(A5, INPUT);
}

void readfft() {
  while(1) { // reduces jitter
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
    Serial.println("start");
//    for (byte i = 0 ; i < FFT_N/2 ; i++) {
//       Serial.print(fft_log_out[i]); // send out the data
//       Serial.print("\t");
//    }
//     Serial.println(" ");

    int maxbin = 2;
    for (int i=2; i<FFT_N/2;i++){   // start at a bin above 0 so that DC IR light is not compared
      if (fft_log_out[i]>fft_log_out[maxbin]){
        maxbin = i;
      }
    }
    int binWhistle = 6;       // 660Hz BIN
    int binIRHat = 44;        // 6.08kHz BIN
    int binIRDecoy = 122;     // 18kHz BIN

    for (int i=binWhistle-2;i<binWhistle+2;i++){    // search in range of plus and minus 2
      int threshold = 120;
      if (fft_log_out[i]>threshold){
        Serial.print("Whistle detected!");
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
}

void loop(){
  readfft();
}
