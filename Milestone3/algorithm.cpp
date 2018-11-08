/*
An algorithm to decide maze route. Please use with global
maze information variable.

*/

// Initially, unexplored squares are set to 8b1111111.
#define MAZEX = 9;
#define MAZEY = 9;
#include <stack>;
#include <queue>;
int [9][9] value;
byte [9][9] searchExplored;
// Maze origin.
int startX = 0;
int startY = 0;
stack dfsStack; // used in DFS
queue bfsQueue; // used in BFS

// easy thing: BFS of nearest unexplored nodes
// cooler thing: shortest path to highest value node

int distance_L1(int x1, int y1, int x2, int y2){
  // L1 distance for graph weighting
  return (abs(y2-y1)+abs(x2-x1));
}

void byteifyCoordinate(int x, int y){
  // helper function to convert (x,y) coordinate to
  // encoded xxxxyyyy byte.
  return (x<<4|y); // @ all: no derp?
}


void unexploredBFS (int x, int y){
  if (!bfsQueue.empty() ||!dfsStack.empty()){
    return
  }

  // BFS for unexplored maze
  // TODO: Make decideRoute work with this.
  // Idea: Go to first out of queue. Update maze. Do BFS again. Lol!
  bfsQueue.push_back(byteifyCoordinate(x,y));
  while (!bfsQueue.empty()){
    // The following location 'first' is going to be popped forever if
    // there are no unexplored nodes: no need to return if dead end. Then,
    // stack will be empty and iterations will end once everything is explored
    byte first =  bfsQueue.pop_front();
    int x_coord = 0b11110000 & (first>>4);
    int y_coord = 0b00001111 & first;
    if (!searchExplored[x_coord][y_coord]){
      searchExplored[x_coord][y_coord] = true;
      if (x+1<MAZEX && maze[x+1][y] == 0b11111111){
        bfsQueue.push_back(byteifyCoordinate(x+1,y));
      }

      else if (x-1>=0 && maze[x-1][y] == 0b11111111){
        bfsQueue.push_back(byteifyCoordinate(x-1,y));
      }

      else if (y+1<MAZEY && maze[x][y+1] == 0b11111111){
        bfsQueue.push_back(byteifyCoordinate(x+1,y+1));
      }

      else if (y-1>=0 && maze[x][y-1] == 0b11111111){
        bfsQueue.push_back(byteifyCoordinate(x,y-1));
      }
    }
  }

}
void unexploredDFS (int x, int y){
  // DFS for unexplored maze
  // TODO: Make decideRoute work with this.
  // Idea: Go to stack pop. Update maze. Do DFS again. Lol!
  if (!bfsQueue.empty() ||!dfsStack.empty()){
    return; // @Hojung: is this ok in CPP
  }

  /* PSEUDOCODE
  procedure DFS-iterative(G,v):
        let S be a stack
        S.push(v)
        while S is not empty
          v = S.pop()
            if v is not labeled as discovered:
                label v as discovered
                for all edges from v to w in G.adjacentEdges(v) do
                    S.push(w)
  */

  dfsStack.push(byteifyCoordinate(x,y));
  while (!dfsStack.empty()){
    // The following location 'first' is going to be popped forever if
    // there are no unexplored nodes: no need to return if dead end. Then,
    // stack will be empty and iterations will end once everything is explored
    byte first =  dfsStack.pop();
    int x_coord = 0b11110000 & (first>>4);
    int y_coord = 0b00001111 & first;
    if (!searchExplored[x_coord][y_coord]){
      searchExplored[x_coord][y_coord] = true;
      if (x+1<MAZEX && maze[x+1][y] == 0b11111111){
        dfsStack.push(byteifyCoordinate(x+1,y));
      }

      else if (x-1>=0 && maze[x-1][y] == 0b11111111){
        dfsStack.push(byteifyCoordinate(x-1,y));
      }

      else if (y+1<MAZEY && maze[x][y+1] == 0b11111111){
        dfsStack.push(byteifyCoordinate(x,y+1));
      }

      else if (y-1>=0 && maze[x][y-1] == 0b11111111){
        dfsStack.push(byteifyCoordinate(x,y-1));
      }
    }
  }
}

void decideRouteDFS(){
  // use DFS stack from x,y origin to stack pop
  byte goal = dfsStack.pop();


}

void updateUnexploredQueue(){
  // if a square is explored remove it from the unexplored
  // queue

}
void assignValue(int x, int y){
  // TODO:
  // Would we rather have many redundant
  // arithmetic operations or a 81-byte array
  // refilled several times
  // but with not *as* many operations

  // note that LABron will continuously update global map


  int neighborSum = 0;
  if ((x<0 || y<0 || x>MAZEX || y>MAZEY){ // if valid coordinate
      return 0;
  }
  if (maze[x-1][y] == 0b11111111){
      neighborSum = neighborSum + assignValue(x-1,y);
  }
  if (maze[x+1][y] == 0b11111111){
      neighborSum = neighborSum +  assignValue(x+1,y);
  }
  if (maze[x][y+1] == 0b11111111){
      neighborSum = neighborSum +  assignValue(x,y+1);
  }
  if (maze[x-1][y-1] == 0b11111111){
      neighborSum = neighborSum +  assignValue(x,y-1);
  }
  return sum;
}
//   else{
//   // TODO: Complete later for more robust algorithm!!
//   if (maze[x][y] == 0b11111111){ // maze is unexplored
//     return assignValue(x,y+1)
//     + assignValue(x,y-1)
//     + assignValue(x+1,y)
//     + assignValue(x-1,y);
//   }
//   else{
//     return assignValue(x,y+1)
//     + assignValue(x,y-1)
//     + assignValue(x+1,y)
//     + assignValue(x-1,y);
//     }
//   }
// }


void initializeUnexplored(int initX, int initY){
  // Initially, all squares are unexplored from initial position
  // (initX, initY)
  for (int x=0; x<MAZEX; x++){
    for (int y=0; y<MAZEY; y++){
      // maze indexed (x,y) with bits 7-4 = x, 3-0 = y
      // push (x,y) unexplored
      searchExplored[x][y] = false;
    }
  }
}
