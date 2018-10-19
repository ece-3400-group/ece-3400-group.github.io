// we do not need two packets 
byte maze [] [] ; // local map of the 9x9 maze, (0,0) is top left corner
byte positionPacket; // bits 0-3 encode x-coordinate, bits 4-7 encode y-coordinate
byte metaPacket; // bits 0-3 NSEW wall ,bits 4-7 encode treasure information (see below) 
                 // In the local map, each coordinate from positionPacket is used 
                 // to refer to a 1 byte metaPacket
                 
// Assume we begin at (0,0) facing NORTH
// Define a bunch of vectors that can be shifted and  ORd together to create encoding

// Positional macros
#define UNKNOWN 0b0000;

// Wall macros
#define NORTH 0b1000;
#define SOUTH 0b0100;
#define WEST 0b0010;
#define EAST 0b0001;

// Treasure shape
#define NOSHAPE 0b00;
#define TRIANGLE 0b01;
#define CIRCLE 0b10;
#define SQUARE 0b11;

// Treasure color
#define NOCOLOR 0b00;
#define RED 0b01;
#define GREEN 0b10;
#define BLUE 0b11;



byte metaPacketEncode(byte wallDirection, byte shape, byte color){
  // With a known wallDdirection,shape, and color, output a metaPacket encoding
  return (direction<<4)|(shape<<2)|color;
}

void metaPacketDecode(byte metaPacket){
  // With a known wallDdirection,shape, and color, output a metaPacket encoding
  byte wallDirection = metaPacket[4:7];
  byte shape = metaPacket[2:3];
  byte color = metaPacket[0:1];
  return NULL;
}

void mazeUpdate(int x, int y, byte metaPacket){
  // Store metaPacket at maze position (x,y). Decode upon access for memory efficiency(?)
  maze[x][y] = metaPacket;
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
