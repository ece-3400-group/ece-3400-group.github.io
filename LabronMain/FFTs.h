#ifndef FFT_H
#define FFT_H
#include <Arduino.h>

#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft
#define ADC0_FFT 0x40
#define ADC1_FFT 0x41
#define ADC2_FFT 0x42
#define ADC3_FFT 0x43
#define ADC4_FFT 0x44

void setupFFT();
byte readFFT(int adcPinNum = ADC4_FFT);
byte isFFTPeak();
void debugFFT();
void displayLedFFT( byte fftResult, int audioPin, int hatPin, int decoyPin );

#endif //FFT_H
