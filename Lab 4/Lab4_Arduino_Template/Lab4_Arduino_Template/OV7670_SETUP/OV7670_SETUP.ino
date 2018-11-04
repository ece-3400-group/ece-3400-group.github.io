#include <Wire.h>

// These are register address definitions from the manual and Lab 4 prelab
 /*write this in hex (eg. 0xAB) 42 and 43 decimal  = 0100|0010 */ 
#define OV7670_I2C_ADDRESS 0x21 
int RESETREG = 0x12; 
int SCALINGREG = 0x3E; 
int CLKREG = 0x11; 
int RESOLREG = 0x12; // same as RESETREG with different important bits for control
int XCOLBARREG = 0x70;
int YCOLBARREG = 0x71; 
int RGB_REG = 0x40;

int KEY_REGISTERS[] = {RESETREG, SCALINGREG, CLKREG, RESOLREG, XCOLBARREG, YCOLBARREG}; 
int read_vals [6]; 
int KEY_REGISTERS_LENGTH = sizeof(KEY_REGISTERS)/sizeof(KEY_REGISTERS[0]); // lol i am no0b

// Things about UART
// RX 0 TX 1: Do not use pins 0 and 1

///////// Main Program //////////////
void setup() {
  Serial.begin(9600);
  Serial.println("begin");
  Serial.println("Address is ");
  Serial.println(OV7670_I2C_ADDRESS, BIN);
  setupCamera();
}

void loop(){

 }

///////// Function Definition //////////////
void setupCamera(){
  // TODO: READ KEY REGISTERS
  Wire.begin();

  for ( int i = 0; i < 6; i++ ){
    read_vals[i] = read_register_value(KEY_REGISTERS[i]);
  }
  // sets up camera to output in the format we're using
  setup555();
  rgbSetup();
  
  read_key_registers();
  Serial.print("RESETREG = "); Serial.print(RESETREG, HEX); Serial.print(" "); Serial.println(read_vals[0], BIN);
  Serial.print("SCALINGREG = ");  Serial.print(SCALINGREG, HEX); Serial.print(" ");Serial.println(read_vals[1], BIN);
  Serial.print("CLKREG Register = ");  Serial.print(CLKREG, HEX); Serial.print(" ");Serial.println(read_vals[2], BIN);
  Serial.print("RESOLREG Register = ");  Serial.print(RESOLREG, HEX); Serial.print(" ");Serial.println(read_vals[3], BIN);
  Serial.print("XCOLBARREG Register = ");  Serial.print(XCOLBARREG, HEX); Serial.print(" ");Serial.println(read_vals[4], BIN);
  Serial.print("YCOLBARREG Register = "); Serial.print(YCOLBARREG, HEX); Serial.print(" "); Serial.println(read_vals[5], BIN);
  set_color_matrix();
}

void rgbSetup() {
  delay(100);
    byte data = 0b00000110; // Output for RGB format 
    OV7670_write_register(RESETREG, data);
}

void setup555() {
  delay(100);
    byte data = 0b11110000; 
    OV7670_write_register(RGB_REG, data);
}


void read_key_registers(){
  /*TODO: DEFINE THIS FUNCTION*/
  // Update global read_vals array corresponding to defined important registers' values

  for (int i=0; i<KEY_REGISTERS_LENGTH; i++){
    byte readVal = read_register_value(KEY_REGISTERS[i]); 
    read_vals[i] = readVal; 
  }
}


byte read_register_value(int register_address){
  byte data = 0;
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Wire.write(register_address);
  Wire.endTransmission();
  Wire.requestFrom(OV7670_I2C_ADDRESS,1);
    Serial.println("TX STARTED");
  while(Wire.available()<1);
  Serial.println("Wire was avalirable");
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
