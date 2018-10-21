#ifndef MAZE
#define MAZE
#include <Arduino.h>

// TODO: EXTERN CONSTS based on Lab3/sendMazeInfo.ino
extern byte direction;  // global current absolute cardinal direction 00 = North, 11 = South, 01 = East, 10 = West
extern int currentX; // current X coordinate
extern int currentY; // current Y coordinate
extern byte maze [9][9];  // global maze stored with metadata
void updateMaze(int x, int y, byte metaPacket); // store metaPacket in 2D array coordinate location x,y
void updateDirection(byte decidedRoute);
byte getMazeInfo(int x, int y);
#endif // MAZE
