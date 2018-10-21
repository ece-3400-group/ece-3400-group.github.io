#ifndef FFT_H
#define FFT_H
#include <Arduino.h>

/******************* [ FFT Settings ] **************************/
#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft
// Bins from running the FFT
#define binWhistle  6       
#define binIRHat   44     
#define binIRDecoy 122 
// ADC Pin options
#define ADC0_FFT 0x40
#define ADC1_FFT 0x41
#define ADC2_FFT 0x42
#define ADC3_FFT 0x43
#define ADC4_FFT 0x44
#define ADC5_FFT 0x45

/******************* [ Bit Masks ] **************************/
#define AUDIO_MASK 0b0001
#define IRHAT_MASK 0b0010
#define DECOY_MASK 0b0100

/******************* [ Threshold Values ] **************************/
#define audioThreshold 100
#define hatThreshold   50
#define decoyThreshold 100

/******************* [ Pin Definitions ] **************************/
#define MASTER_FFT_PIN A5
#define AUDIO_PIN      2
#define IRHAT_PIN      7

/******************* [ Function Prototypes ] **************************/
void setupFFT();
byte waitForStart();
byte readFFT(int adcPinNum = ADC5_FFT);
byte isFFTPeak();
void setupDebugFFT();
void debugFFT();
// Do **NOT** call with radios on the Arduino
void displayLedFFT( byte fftResult, int audioPin, int hatPin );

#endif //FFT_H
