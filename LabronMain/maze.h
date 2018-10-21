#ifndef MAZE
#define MAZE
#include <Arduino.h>

void updateMaze(int x, int y, byte metaPacket);
byte getMazeInfo(int x, int y);

#endif // MAZE
