#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21
// Register definitions
#define NUM_REGISTERS 7
#define SCALING_COM3 0x0C
#define RESET_COM7   0x12
// Might not need noise one
#define NOISE_COM9   0x14 
#define OUTPUTRANGE_COM15 0x40
#define COLORBAR_COM17 0x42
#define CLKRC_USEEXTCLK 0x11
#define MVFP_FLIP 0x1E

int KEY_REGISTERS[] = {SCALING_COM3, RESET_COM7, NOISE_COM9, OUTPUTRANGE_COM15, COLORBAR_COM17, CLKRC_USEEXTCLK, MVFP_FLIP}; 
int readVals[NUM_REGISTERS];

///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Beginning");

  read_key_registers();

  Serial.println("\nFinished reading registers\n");
  delay(100);
  
  write_key_registers();

  Serial.println("\nFinished writing registers\n");
  
  read_key_registers();
}

void loop(){
 }


///////// Function Definition //////////////
void write_key_registers(){
  byte data = 0b00000000;
  byte resolution      = 0b00001000; // Sets resolution
  byte colorbar        = 0b00000010; // color bar
  byte rgbformat       = 0b00000100; // RGB format enabled
  byte reset_regs      = 0b10000000; // resets registers
  byte enable_scaling      = 0b00001000; // enables scaling
  byte agcfreeze           = 0b00000001; // freezes agc
  byte rgbformat555        = 0b11110000; // largest output range and RGB555 format
  byte enablecolorbartest  = 0b00001000; // enable the color bar test
  byte enabledoubleclk     = 0b10000000; // enable double clock
  byte useextclk           = 0b01000000; // use external clock
  byte mirrorimage         = 0b00000000; // mirror image
  for (int i = 0; i < NUM_REGISTERS; i++) {
    if (i==0)      data = (data | enable_scaling); // enables scaling, note BIT7 is reserved
    else if (i==1) data = (data | reset_regs | enable_scaling | rgbformat); // resets registers & enables RGB format
    else if (i==2) data = (data | agcfreeze); // freezes AGC
    else if (i==3) data = (data | rgbformat555);
    else if (i==4) data = (data | enablecolorbartest);
    else if (i==5) data = (data | enabledoubleclk | useextclk);
    else if (i==6) data = (data | mirrorimage);
    OV7670_write_register(KEY_REGISTERS[i], data);
  }

  OV7670_write_register(RESET_COM7, (resolution | colorbar | rgbformat)); // sets resolution
}

void read_key_registers(){
  for (int i=0; i< NUM_REGISTERS; i++ ){
    readVals[i] = read_register_value(KEY_REGISTERS[i]);
  }
  
   Serial.print("SCALING_COM3 = "); Serial.print(SCALING_COM3, HEX); Serial.print(" "); Serial.println(readVals[0], BIN);
  Serial.print("RESET_COM7 = ");  Serial.print(RESET_COM7, HEX); Serial.print(" ");Serial.println(readVals[1], BIN);
  Serial.print("NOISE_COM9 = ");  Serial.print(NOISE_COM9, HEX); Serial.print(" ");Serial.println(readVals[2], BIN);
  Serial.print("OUTPUTRANGE_COM15 = ");  Serial.print(OUTPUTRANGE_COM15, HEX); Serial.print(" ");Serial.println(readVals[3], BIN);
  Serial.print("COLORBAR_COM17r = ");  Serial.print(COLORBAR_COM17, HEX); Serial.print(" ");Serial.println(readVals[4], BIN);
  Serial.print("CLKRC_USEEXTCLK = "); Serial.print(CLKRC_USEEXTCLK, HEX); Serial.print(" "); Serial.println(readVals[5], BIN);
  Serial.print("MVFP_FLIP = "); Serial.print(MVFP_FLIP, HEX); Serial.print(" "); Serial.println(readVals[6], BIN);
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
