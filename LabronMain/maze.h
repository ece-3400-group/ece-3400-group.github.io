#ifndef MAZE
#define MAZE
#include <Arduino.h>

// TODO: EXTERN CONSTS based on Lab3/sendMazeInfo.ino

void updateMaze(int x, int y, byte metaPacket);
byte getMazeInfo(int x, int y);
#endif // MAZE
