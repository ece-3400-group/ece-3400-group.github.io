#include <string.h>

// we do not need two packets 
byte maze [9][9] ; // local map of the 9x9 maze, (0,0) is top left corner
byte positionPacket; // bits 0-3 encode x-coordinate, bits 4-7 encode y-coordinate
byte metaPacket; // bits 0-3 NSEW wall ,bits 4-7 encode treasure information (see below) 
// In the local map, each coordinate from positionPacket is used 
// to refer to a 1 byte metaPacket

// Assume we begin at (0,0) facing NORTH
// Define a bunch of vectors that can be shifted and  ORd together to create encoding

// Positional macros
#define UNKNOWN 0b0000

// Wall macros
#define NORTH 0b1000
#define SOUTH 0b0100
#define WEST 0b0010
#define EAST 0b0001

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

byte metaPacketEncode(byte wallDirection, byte shape, byte color){
  // With a known wallDdirection,shape, and color, output a metaPacket encoding
  return (wallDirection<<4)|(shape<<2)|color;
}

void metaPacketDecode(byte metaPacket){
  // With a known wallDdirection,shape, and color, output a metaPacket encoding
  byte wallWest = getBit(metaPacket, 1, 2);  // extract bin 1 
  byte wallNorth = getBit(metaPacket, 2, 3);  // extract bin 2
  byte wallEast = getBit(metaPacket, 3, 4);  // extract bin 3
  byte wallSouth = getBit(metaPacket, 4, 5);  // extract bin 4
  byte shape = getBit(metaPacket, 5, 7);                // extract bin 5 to 6
  byte color = getBit(metaPacket, 7, 9);                 // extract bin 7 to 8
  byte rows = getBit(metaPacket, 9,13);                 // extract bin 9 to 12
  byte cols = getBit(metaPacket, 13, 17);                 // extract bin 13 to 16
  
  if(WEST == wallWest){West = "true";}
  else{West = "false";}
  
  if(NORTH == wallNorth){North = "true";}
  else{North = "false";}
  
  if(EAST == wallEast){East = "true";}
  else{East = "false";}
  
  if(SOUTH == wallSouth){South = "true";}
  else{South = "false";}

  // treasure shape byte to string
  if(NOSHAPE == shape){TShape = "none";}  
  else if(TRIANGLE == shape){TShape = "triangle";}  
  else if(CIRCLE == shape){TShape = "circle";}
  else if(SQUARE == shape){TShape = "square";}
  else{TShape = "none";}
  
  // treasure color byte to string
  if(NOCOLOR == color){TColor = "none";}  
  else if(RED == color){TColor = "red";}  
  else if(GREEN == color){TColor = "green";}
  else if(BLUE == color){TColor = "blue";}
  else{TColor = "none";}
}

void mazeUpdate(int x, int y, byte metaPacket){
  // Store metaPacket at maze position (x,y). Decode upon access for memory efficiency(?)
  maze[x][y] = metaPacket;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void outputGUI(){
  Serial.print(rows);
  Serial.print(", ");
  Serial.print(cols);
  Serial.print(", ");
  Serial.print("west=");
  Serial.print(West);
  Serial.print(", ");
  Serial.print("north=");
  Serial.print(North);
  Serial.print(", ");
  Serial.print("east=");
  Serial.print(East);
  Serial.print(", ");
  Serial.print("south=");
  Serial.print(South);
  Serial.print(", ");
  Serial.print("tshape=");
  Serial.print(TShape);
  Serial.print(", ");
  Serial.print("tcolor=");
  Serial.println(TColor);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  outputGUI();
}

