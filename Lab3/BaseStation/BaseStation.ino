#include <SPI.h>
#include "radios.h"
#include <string.h>


byte _pos = 0b11111111;
byte _meta = 0b11000000;
unsigned long  _receive = 0b0000000000000000;

// we do not need two packets 
byte maze [9][9] ; // local map of the 9x9 maze, (0,0) is top left corner
byte positionPacket; // bits 0-3 encode x-coordinate, bits 4-7 encode y-coordinate
byte metaPacket; // bits 0-3 NSEW wall ,bits 4-7 encode treasure information (see below) 
// In the local map, each coordinate from positionPacket is used 
// to refer to a 1 byte metaPacket

// Assume we begin at (0,0) facing NORTH
// Define a bunch of vectors that can be shifted and  ORd together to create encoding

byte metaPacketNorthEast;
// In the local map, each coordinate from positionPacket is used
// to refer to a 1 byte metaPacket
// bit 0 = NorthEast identifier
// bit 1 = 0 if North, 1 if East
// bit 2 = 1 if wall detected here
// bit 3 = 1 if  other robot detected
// bit 4 = 1 if visited before
// bit 5-6 = treasure shape (00 NONE, 01 TRIANGLE, 10 CIRCLE, 11 SQUARE)
// bit 7-8 = treasure color (00 NONE, 01 RED, 10 BLUE, 11 GREEN)

byte metaPacketSouthWest;

// In the local map, each coordinate from positionPacket is used
// to refer to a 1 byte metaPacket
// bit 1 =SouthWest identifier
// bit 1 = 0 if South, 1 if West
// bit 2 = 1 if wall detected here
// bit 3 = 1 if other robot detected
// bit 4 = 1 if visited before
// bit 5-6 = treasure shape (00 NONE, 01 TRIANGLE, 10 CIRCLE, 11 SQUARE)
// bit 7-8 = treasure color (00 NONE, 01 RED, 10 BLUE, 11 GREEN)

// Positional macros
#define UNKNOWN 0b0000

// Wall macros
#define NORTH 0b01
#define EAST 0b01
#define SOUTH 0b01
#define WEST 0b01


// Treasure shape
#define NOSHAPE 0b00
#define TRIANGLE 0b01
#define CIRCLE 0b10
#define SQUARE 0b11

// Treasure color
#define NOCOLOR 0b00
#define RED 0b01
#define GREEN 0b10
#define BLUE 0b11

byte WallDirection[4] = {
  NORTH, SOUTH, WEST, EAST};
byte Treasure[4] = {
  NOSHAPE, TRIANGLE, CIRCLE, SQUARE};
byte TreasColor[4] = {
  NOCOLOR, RED, GREEN, BLUE};

String North = String("false");
String South = String("false");
String East = String("false");
String West = String("false");
String Robot = String("false");
String TShape = String("None");
String TColor = String("None");

int rows = 0;
int cols = 0;

int getBit(int n, int s, int f){
  // extract binary bins from a byte. n=Binary Byte; s=Start Bin; f=Final Bin;
  int result = 0;
  for(int k=0; k<(f-s); k++){
    int t = (n & (1<<(k+s)))!=0;
    result |= t<<k;
  }
  return result;
}

void PrintByte(byte obj){
 for(int i = 7; i>-1; --i){
   Serial.print(getBit(obj, i,i+1));
 } 
 Serial.println();
}

byte metaPacketEncode(byte wallDirection, byte shape, byte color){
  // With a known wallDdirection,shape, and color, output a metaPacket encoding
  return (wallDirection<<4)|(shape<<2)|color;
}

void metaPacketDecode_Position(byte metaPacket){
  byte prows = getBit(metaPacket, 4, 8);  // extract bin 1 
  byte pcols = getBit(metaPacket, 0, 4);  // extract bin 1 
  if (prows == 15){
   rows = 1; 
  }else{
    rows = prows;
  }
  cols = pcols;
}

void wrongDecoder_Position(){
  if( (rows == 0) && (cols == 0)){
    rows = 2;
    cols = 1;
  }
  if( (rows == 1) && (cols == 0)){
    rows = 2;
    cols = 0;
  }
  if( (rows == 1) && (cols == 1)){
    rows = 1;
    cols = 0;
  }
  if( (rows == 0) && (cols == 1)){
    rows = 1;
    cols = 1;
  }
  if( (rows == 0) && (cols == 2)){
    rows = 0;
    cols = 1;
  }
  if( (rows == 1) && (cols == 2)){
    rows = 0;
    cols = 0;
  }
}

void metaPacketDecode_Treasure(byte metaPacket){
  // With a known wallDdirection,shape, and color, output a metaPacket encoding  
  byte wallWest = getBit(metaPacket, 7, 8);  // extract bin 1 
  byte wallNorth = getBit(metaPacket, 6, 7);  // extract bin 2
  byte wallEast = getBit(metaPacket, 5, 6);  // extract bin 3
  byte wallSouth = getBit(metaPacket, 4, 5);  // extract bin 4
  byte shape = getBit(metaPacket, 2, 4);                // extract bin 5 to 6
  byte color = getBit(metaPacket, 0, 2);                 // extract bin 7 to 8
//  Serial.print("Decoder_Treasuer : ");
//  Serial.print(wallWest);
//  Serial.print(wallNorth);
//  Serial.print(wallEast);
//  Serial.print(wallSouth);
//  Serial.print(shape);
//  Serial.println(color);


  if(WEST == wallWest){
    West = "true";
  }
  else{
    West = "false";
  }

  if(NORTH == wallNorth){
    North = "true";
  }
  else{
    North = "false";
  }

  if(EAST == wallEast){
    East = "true";
  }
  else{
    East = "false";
  }

  if(SOUTH == wallSouth){
    South = "true";
  }
  else{
    South = "false";
  }

  // treasure shape byte to string
  if(NOSHAPE == shape){
    TShape = "none";
  }  
  else if(TRIANGLE == shape){
    TShape = "triangle";
  }  
  else if(CIRCLE == shape){
    TShape = "circle";
  }
  else if(SQUARE == shape){
    TShape = "square";
  }
  else{
    TShape = "none";
  }

  // treasure color byte to string
  if(NOCOLOR == color){
    TColor = "none";
  }  
  else if(RED == color){
    TColor = "red";
  }  
  else if(GREEN == color){
    TColor = "green";
  }
  else if(BLUE == color){
    TColor = "blue";
  }
  else{
    TColor = "none";
  }
}

void mazeUpdate(int x, int y, byte metaPacket){
  // Store metaPacket at maze position (x,y). Decode upon access for memory efficiency(?)
  maze[x][y] = metaPacket;
}

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(57600);
  setupRadios();
}

void outputGUI(){
  Serial.print(rows);
  Serial.print(",");
  Serial.print(cols);
  Serial.print(",");
  Serial.print("west=");
  Serial.print(West);
  Serial.print(",");
  Serial.print("north=");
  Serial.print(North);      //chaged
  Serial.print(",");
  Serial.print("east=");
  Serial.print(East);
  Serial.print(",");
  Serial.print("south=");
  Serial.print(South);
//  Serial.print(",");
//  Serial.print("tshape=");
//  Serial.print(TShape);
//  Serial.print(",");
//  Serial.print("tcolor=");
//  Serial.println(TColor);
  Serial.println("");

}
int p =0;

void loop(){
  _receive = packetTransmission(_pos, _meta);
  //  Serial.println("send pos&meta package");
  //  Serial.print("receiving package ");
  //Serial.println(_receive);
//   Serial.print("Printing first byte  :");
   PrintByte(_receive>>8);
//   Serial.print("Printing second byte  :");
   PrintByte(getBit(_receive, 0, 8));
  //int  package[6] = {208, 352, 4400, 4288, 8336, 8560 };    // testing GUI binary
  
  metaPacketDecode_Position(_receive>>8);  
//  Serial.println(_receive);
  metaPacketDecode_Treasure(getBit(_receive, 0, 8));
  //Serial.println("test");
  //wrongDecoder_Position();
  if (_receive != 0){
    outputGUI();
  }
//  p++;
//  if(p==6) {
//    p = 0;
//    delay(500);
//    Serial.println("reset");
//  }
  //outputGUI();
  //Serial.println("========================End of the loop==============================");
  delay(500);
}
