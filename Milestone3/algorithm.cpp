/*
An algorithm to decide maze route. Please use with global
maze information variable.

*/

// Initially, unexplored squares are set to 8b1111111.
#define MAZEX = 9;
#define MAZEY = 9;
#include <stack>;
byte [9][9] searchExplored;
// Maze origin.
int startX = 0; //@Hojung need to make MAZEY, MAZEX, startX, startY accessible across all cpp files in LabronMain :)
int startY = 0;
stack dfsStack; // used in DFS


int distance_L1(int x1, int y1, int x2, int y2){
  // L1 distance for graph weighting
  return (abs(y2-y1)+abs(x2-x1));
}

void clearStack (stack <byte> p){
  // get rid of everything in a stack
  for (int i=0; i<p.size(); i++){
    p.pop(); // @ Hojung: ok way to clear stack?
  }
}

void byteifyCoordinate(int x, int y){
  // helper function to convert (x,y) coordinate to
  // encoded xxxxyyyy byte.
  x = x & 0b00001111;
  y = y & 0b00001111;
  return (x<<4|y); // @ all: no derp?
}


void unexploredDFS (int x, int y){

  // DFS for unexplored maze starting at (x,y)
  // TODO: Make decideRoute work with this.
  initializeUnexplored(); // check back with global maze to see what's been explored
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
      searchExplored[x_coord][y_coord] = true; // not necessarily valid path (can be obstructed later) so
      // not updated on global maze yet

      if (x+1<MAZEX && !searchExplored[x+1][y]){ //has east neighbor been explored yet?
        dfsStack.push(byteifyCoordinate(x+1,y));
      }

      if (x-1>=0 && !searchExplored[x-1][y]){ // has west neighbor been explored yet
        dfsStack.push(byteifyCoordinate(x-1,y));
      }

      if (y+1<MAZEY && !searchExplored[x][y+1]){ // has north neighbor been explored yet
        dfsStack.push(byteifyCoordinate(x,y+1));
      }

      if (y-1>=0 && !searchExplored[x][y-1]){ // has south neighbor been explored yet
        dfsStack.push(byteifyCoordinate(x,y-1));
      }
      else{ // end when there isn't an unexplored node to go to
        break;
      }
    }
  }
}

bool mazeCovered(){
  // Return if the maze is fully
  for (int i=0; i<MAZEX; i++){
    for (int j=0; j<MAZEY; j++){
      if (!searchExplored[i][j]){
        return false; // any position has not been DFSd
      }
    }
  }
  return true; // all positions have been DFSd
}


void initializeUnexplored(int initX, int initY){
  // Initially, all squares are unexplored from initial position
  // (initX, initY)
  for (int x=0; x<MAZEX; x++){
    for (int y=0; y<MAZEY; y++){
      // maze indexed (x,y) with bits 7-4 = x, 3-0 = y
      // push (x,y) unexplored
      if (maze[x][y] == 0b11111111){
        searchExplored[x][y] = false;
        // these parts of the global maze are yet to be reached
          }
      else{
        searchExplored[x][y] = true;
      }
    }
  }
}
