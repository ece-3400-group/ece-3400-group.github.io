#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21
#define CLKRC 0x11
#define COM3 0x0C
#define COM7   0x12
#define COM15 0x40
#define COM17 0x42
#define MVFP 0x1E

// Register definitions
#define NUM_REGISTERS 7
// Might not need noise one
#define COM9   0x14 



//int KEY_REGISTERS[] = {RESET_COM7, // BIT 6 reserved
//                        SCALING_COM3, // BIT 0,1,7 reserved
//                        NOISE_COM9, // BIT 1,2,3,7 reserved
//                        OUTPUTRANGE_COM15, // BIT 0,1,2,3 reserved
//                        COLORBAR_COM17, // BIT 0,1,2,4,5 reserved
//                        CLKRC_USEEXTCLK, // none reserved
//                        MVFP_FLIP //BIT 0,1,3,6,7 reserved
//                        }; 

///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Beginning");

  OV7670_write_register(COM7 , 0x80);
  delay(100);
  set_color_matrix();
  OV7670_write_register(CLKRC , 0xC0);
  OV7670_write_register(MVFP , 0x30);
  OV7670_write_register(COM3 , 0X08);
  OV7670_write_register(COM7 , 0x0C);
  OV7670_write_register(COM15 , 0XF0);
  OV7670_write_register(COM17 , 0b00000000);
  
  read_key_registers();
}

void loop(){
 }


///////// Function Definition //////////////
void write_key_registers(){
  byte data;
  
  // COM 7
  byte resolution      = 0b00001000; // Sets resolution
  byte colorbar        = 0b00000010; // color bar
  byte rgbformat       = 0b00000100; // RGB format enabled
  byte reset_regs      = 0b10000000; // resets registers

  // COM 3
  byte enable_scaling      = 0b00001000; // enables scaling
  // COM 9 
  byte agcfreeze           = 0b00000001; // freezes agc
  // COM 15
  byte rgbformat555        = 0b11110000; // largest output range and RGB555 format
  // COM 17
  byte enablecolorbartest  = 0b00001000; // enable the color bar test
  // CLKRC
  byte enabledoubleclk     = 0b10000000; // enable double clock
  byte useextclk           = 0b01000000; // use external clock
  // MVFP
  byte mirrorimage         = 0b00000000; // mirror image

//   data = reset_regs;
//   OV7670_write_register(RESET_COM7, data); // initial register reset
//      Serial.print("Data after resetting is " ); Serial.println(data);
//   delay(500);
//   data = (resolution | colorbar | rgbformat);
//   Serial.print("Data is " ); Serial.println(data);
//    OV7670_write_register(RESET_COM7, data); // initial register reset


//  for (int i = 0; i < NUM_REGISTERS; i++) {
//    data = read_register_value(KEY_REGISTERS[i]); // read original value of register
//    if (i==0){
//      data |= reset_regs; // resets registers
//      OV7670_write_register(RESET_COM7, data); // initial register reset
//      delay(100);     
//      data = (read_register_value(KEY_REGISTERS[0]) | resolution | colorbar | rgbformat); // now always OR with default values
//      OV7670_write_register(RESET_COM7, data); // sets resolution
//    }
//    
//    else if (i== 1) {
//       data |= enable_scaling; // enables scaling, note BIT7 is reserved
//    }
//    else if (i==2) data |= agcfreeze; // freezes AGC
//    else if (i==3) data |= rgbformat555;
//    else if (i==4) data |= enablecolorbartest;
//    else if (i==5) data |= (data | enabledoubleclk | useextclk);
//    else if (i==6) data |= mirrorimage;
//    OV7670_write_register(KEY_REGISTERS[i], data);

  OV7670_write_register(0x12 , 0x80);
  //delay(100); // ta told us to do this but it makes it worse 
  //color bar test/ RGB       - value of 4 sets RGB mode, 6 sets color bar and RGB 
  //OV7670_write_register(0x12 , 0x06);
    //CLKRC for external clock
  OV7670_write_register(0x11 , 0xC0);
    //mvfp for mirror and flip screen
  OV7670_write_register(0x1E , 0x30);
  //Com3 for enable scaling
  OV7670_write_register(0x0C , 0x08);
  //COM7 
  OV7670_write_register(0x12 , 0x0C);
  //com15 for RGB format
  OV7670_write_register(0x40 , 0xF0);

//  //com17
  OV7670_write_register(0x42 , 0x0C);
    Serial.print(read_register_value(COM17), HEX);

  

}
void read_key_registers(){  
  Serial.print("RESET_COM7 = ");  Serial.print(COM7, HEX); Serial.print(" ");Serial.println( read_register_value(COM7), HEX);
  Serial.print("SCALING_COM3 = "); Serial.print(COM3, HEX); Serial.print(" "); Serial.println(read_register_value(COM3), HEX);
  Serial.print("NOISE_COM9 = ");  Serial.print(COM9, HEX); Serial.print(" ");Serial.println(read_register_value(COM9), HEX);
  Serial.print("OUTPUTRANGE_COM15 = ");  Serial.print(COM15, HEX); Serial.print(" ");Serial.println(read_register_value(COM15), HEX);
  Serial.print("COLORBAR_COM17 = ");  Serial.print(COM17, HEX); Serial.print(" ");Serial.println(read_register_value(COM17), HEX);
  Serial.print("CLKRC_USEEXTCLK = "); Serial.print(CLKRC, HEX); Serial.print(" "); Serial.println(read_register_value(CLKRC), HEX);
  Serial.print("MVFP_FLIP = "); Serial.print(MVFP, HEX); Serial.print(" "); Serial.println(read_register_value(MVFP), HEX);
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
