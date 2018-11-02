#include <Wire.h>

// These are register address definitions from the manual and Lab 4 prelab
#define OV7670_I2C_ADDRESS 0x42 << 1;  /*write this in hex (eg. 0xAB) 42 and 43 decimal  = 0100|0010 */ 
int RESETREG = 0x12; 
int SCALINGREG = 0x3E; 
int CLKREG = 0x11; 
int RESOLREG = 0x12; // same as RESETREG with different important bits for control
int XCOLBARREG = 0x70;
int YCOLBARREG = 0x71; 

int KEY_REGISTERS [] = {RESETREG, SCALINGREG, CLKREG, RESOLREG, XCOLBARREG, YCOLBARREG}; 
int read_vals [6]; 
int KEY_REGISTERS_LENGTH = sizeof(KEY_REGISTERS)/sizeof(KEY_REGISTERS[0]); // lol i am no0b

// Things about UART
// RX 0 TX 1: Do not use pins 0 and 1



///////// Main Program //////////////
void setup() {
  
  Serial.begin(9600);
  setupCamera();
  
}

void setupCamera(){
  // TODO: READ KEY REGISTERS
  Wire.begin();
  int regAddress = 0b00010010; 
  byte readVal = read_register_value(regAddress);
  
  // sets up camera to output in the format we're using
  rgbSetup();
  555setup();
  
  read_key_registers();
  set_color_matrix();
}

void rgbSetup() {
	int regAddress = 0b00010010; 
	delay(100);
  	byte data = 0b00000101; 
  	OV7670_write_register(regAddress, data);
}

void 555setup() {
	int regAddress = 0b01000000; 
	delay(100);
  	byte data = 0b00110000; 
  	OV7670_write_register(regAddress, data);
}


void loop(){
  
 }


void setupUART(int baud){
  Serial.begin(baud);
}



///////// Function Definition //////////////
void read_key_registers(){
  /*TODO: DEFINE THIS FUNCTION*/
  // Update global read_vals array corresponding to defined important registers' values
  byte readVal; 
  
  for (i=0; i<KEY_REGISTERS_LENGTH; i++){
    readVal = read_register(KEY_REGISTERS[i]); 
    read_vals[i] = readVal; 
  }
  
}


byte read_register_value(int register_address){
  byte data = 0;
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Wire.write(register_address);
  Wire.endTransmission();
  Wire.requestFrom(OV7670_I2C_ADDRESS,1);
  while(Wire.available()<1);
  data = Wire.read();
  return data;
}


String OV7670_write(int start, const byte *pData, int size){
    int n,error;
    Wire.beginTransmission(OV7670_I2C_ADDRESS);
    n = Wire.write(start);
    if(n != 1){
      return "I2C ERROR WRITING START ADDRESS";   
    }
    n = Wire.write(pData, size);
    if(n != size){
      return "I2C ERROR WRITING DATA";
    }
    error = Wire.endTransmission(true);
    if(error != 0){
      return String(error);
    }
    return "no errors :)";
 }


String OV7670_write_register(int reg_address, byte data){
  return OV7670_write(reg_address, &data, 1);
 }
 

void set_color_matrix(){
    OV7670_write_register(0x4f, 0x80);
    OV7670_write_register(0x50, 0x80);
    OV7670_write_register(0x51, 0x00);
    OV7670_write_register(0x52, 0x22);
    OV7670_write_register(0x53, 0x5e);
    OV7670_write_register(0x54, 0x80);
    OV7670_write_register(0x56, 0x40);
    OV7670_write_register(0x58, 0x9e);
    OV7670_write_register(0x59, 0x88);
    OV7670_write_register(0x5a, 0x88);
    OV7670_write_register(0x5b, 0x44);
    OV7670_write_register(0x5c, 0x67);
    OV7670_write_register(0x5d, 0x49);
    OV7670_write_register(0x5e, 0x0e);
    OV7670_write_register(0x69, 0x00);
    OV7670_write_register(0x6a, 0x40);
    OV7670_write_register(0x6b, 0x0a);
    OV7670_write_register(0x6c, 0x0a);
    OV7670_write_register(0x6d, 0x55);
    OV7670_write_register(0x6e, 0x11);
    OV7670_write_register(0x6f, 0x9f);
    OV7670_write_register(0xb0, 0x84);
}
