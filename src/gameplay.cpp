/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#include "gameplay.h"
#include "cameraObj.h"
#include "cubeObj.h"
#include "goalObj.h"
#include "cubiorObj.h"
#include "collision.h"
#include "mapReader.h"
#include <iostream>
#include <cstdio>
#include <stdlib.h> // for NULL

using namespace std;

int gameplayStartIterator = 0;
bool cubiorPlayable[cubiorCount];
bool goodCollision = true;
CubeObj* collisionMap[maxWidth][maxHeight][maxDepth];
CubeObj* permanentMap[maxWidth][maxHeight][maxDepth];

int currentMapWidth;
int currentMapHeight;
int currentMapDepth;
int cubeCount;

CubiorObj cubior[cubiorCount];
CubeObj cube[maxCubeCount];
GoalObj goal;
CameraObj camera[cubiorCount];
static int movementSpeed = 1;
static int jumpSpeedRatio = 5;
static int rotationSpeed = 10;
int maxJump = 25;
int maxSpeed = 20;
int friction = 1;

int gravity = 2;
int floor = -200;

// Changing game state variables
bool gameplayRunning = true;

// FIXME: This causes lots of lag right now. Intended to keep player inside game though
void keepInBounds(CubeObj* c1) {
    // The bounds are one fewer than the size of the grid
    // this makes checking collision in all directions from a cube never go out of bounds
    if (c1->getX()< (-currentMapWidth/2+mapEdge)*tileSize){c1->setX((-currentMapWidth/2 +mapEdge)*tileSize);}
    if (c1->getX()>=( currentMapWidth/2-mapEdge)*tileSize){c1->setX(( currentMapWidth/2 -mapEdge)*tileSize);}
    if (c1->getY()<-(currentMapHeight/2+mapEdge)*tileSize){c1->setY((-currentMapHeight/2+mapEdge)*tileSize);}
    if (c1->getY()>=(currentMapHeight/2-mapEdge)*tileSize){c1->setY(( currentMapHeight/2-mapEdge)*tileSize);}
    if (c1->getZ()< (-currentMapDepth/2+mapEdge)*tileSize){c1->setZ((-currentMapDepth/2 +mapEdge)*tileSize);}
    if (c1->getZ()>=( currentMapDepth/2-mapEdge)*tileSize){c1->setZ(( currentMapDepth/2 -mapEdge)*tileSize);}
}

void wipeMap(CubeObj* map[][maxHeight][maxDepth]){
  // Wipe collision map, repopulate it
  for (int a=0; a<currentMapWidth; a++) {
  for (int b=0; b<currentMapHeight;b++) {
  for (int c=0; c<currentMapDepth; c++) {
     collisionMap[a][b][c]=0;
  } } }
}

void gameplayStart() {
gameplayStartIterator++;
cout << "gameplayStart iteration " << gameplayStartIterator << endl;
if (gameplayRunning) {

  // Read in a map first!
  MapReader::readMap("./maps/cubiorMap1.cubior");
  currentMapWidth = 20;
  currentMapHeight = 20;
  currentMapDepth = 20;
  cubeCount = (currentMapWidth+1)*(currentMapDepth+1)+6;
  if (currentMapWidth > maxWidth) { currentMapWidth = maxWidth; }
  if (currentMapHeight > maxHeight) { currentMapHeight = maxHeight; }
  if (currentMapDepth > maxDepth) { currentMapDepth = maxDepth; }
  if (cubeCount > maxCubeCount) { cubeCount = maxCubeCount; }

  for (int i=0; i<cubiorCount; i++) {
  // Choose who starts
    cubiorPlayable[i] = false;

  // Start camera!
    camera[i].setPos(0,-165,-1550);
    camera[i].alwaysFollow(&cubior[i]);

  // Cubior Start States!
    cubior[i].setPos(-200*(i - cubiorCount/2)+0,100,400);
    cubior[i].moveX(3);
    cubior[i].moveY(3);
    cubior[i].moveZ(3);
    cubior[i].setHappiness(1.0-i*1.0/cubiorCount);
  }

  // Then ensure at least P1 is playing
  cubiorPlayable[0] = true;

  // and Cube Obstacle start states
  for (int i=0; i<cubeCount; i++) {
//    cube[i].setPos(-100*i+00,-100,0);
    cube[i].setPermalock(true);
  }
  for (int x=0; x<=currentMapWidth; x++) {
    for (int z=0; z<=currentMapDepth; z++) {
      cube[x*(currentMapDepth+1)+z].setPos(100*(x-currentMapWidth/2),-200,100*(z-currentMapDepth/2));
    }
  }
  cube[cubeCount-6].setPos(-100*0,-000,000);
  cube[cubeCount-5].setPos(-100*2,-100,000);
  cube[cubeCount-4].setPos(-100*1,-100,000);
  cube[cubeCount-3].setPos(-100*0,-100,000);
  cube[cubeCount-2].setPos(-100*1,-100,100);
  cube[cubeCount-1].setPos(-100*0,-100,100);
  
  // Then the goal
  goal.setPos(000,700,-000);

  // Then populate permamap
  wipeMap(permanentMap);
  // ... with permanent Cubes
  for (int i = 0; i<cubeCount; i++) {
    cube[i].tick();
    //keepInBounds(&cube[i]);
    addToCollisionMap(&cube[i], permanentMap);
  }
  // Then set their neighbors, for more efficient rendering
  for (int i = 0; i<cubeCount; i++) {
    findNeighbors(&cube[i], permanentMap);
  }
  
}
}

void gameplayLoop() {
if (gameplayRunning) {

  wipeMap(collisionMap);

  // Run main tick loop for all Cubiors...
  for (int i = 0; i<cubiorCount; i++) {
    if (cubiorPlayable[i]) {
      cubior[i].tick();
      keepInBounds(&cubior[i]);
      addToCollisionMap(&cubior[i], collisionMap);
    }
  }
  // and the goal
  goal.tick();
  addToCollisionMap(&goal, collisionMap);

  // Then check collision against all other obstacles (cubes/cubiors)
  for (int i = 0; i<cubiorCount; i++) {
    if (cubiorPlayable[i]) {
    
      int cX = getCollisionMapSlot(&cubior[i],0);
      int cY = getCollisionMapSlot(&cubior[i],1);
      int cZ = getCollisionMapSlot(&cubior[i],2);

      if (goodCollision) {
        explodingDiamondCollision(&cubior[i],permanentMap,cX,cY,cZ);
      } else {
        unintelligentCollision(&cubior[i],permanentMap,cX,cY,cZ);
      }

      // Update c's for non-permanent-item collision
      cX = getCollisionMapSlot(&cubior[i],0);
      cY = getCollisionMapSlot(&cubior[i],1);
      cZ = getCollisionMapSlot(&cubior[i],2);

      if (goodCollision) {
        explodingDiamondCollision(&cubior[i],collisionMap,cX,cY,cZ);
      } else {
      unintelligentCollision(&cubior[i],collisionMap,cX,cY,cZ);
      }
    }
  }
  
  // Finally, make camera catchup
  for (int i=0; i<cubiorCount; i++) {
    if (cubiorPlayable[i]){
      camera[i].tick();
    }
  }
}
}

// Takes cubior, and its slot, then checks collision
// called exploding diamond because it checks on an expanding radius
void explodingDiamondCollision(CubeObj* i, CubeObj* m[][maxHeight][maxDepth], int cX, int cY, int cZ) {
  // Then check slots in relation to cXYZ
  for (int x = 0; x<mapEdge; x++) {
    for (int y = 0; y<=x; y++) {
      for (int z = 0; z<=x; z++) {
        if (x >= y+z) {
         Collision::checkAndBounce(i,m[cX+x-(y+z)][cY+y][cZ+z]);
         Collision::checkAndBounce(i,m[cX-x+(y+z)][cY+y][cZ+z]);
         Collision::checkAndBounce(i,m[cX+x-(y+z)][cY-y][cZ+z]);
         Collision::checkAndBounce(i,m[cX-x+(y+z)][cY-y][cZ+z]);
         Collision::checkAndBounce(i,m[cX+x-(y+z)][cY+y][cZ-z]);
         Collision::checkAndBounce(i,m[cX-x+(y+z)][cY+y][cZ-z]);
         Collision::checkAndBounce(i,m[cX+x-(y+z)][cY-y][cZ-z]);
         Collision::checkAndBounce(i,m[cX-x+(y+z)][cY-y][cZ-z]);
        }
      }
    }
  }
}

// Takes cubior, and its slot, then checks collision
// called unintelligent because it checks by array slot, not distance
void unintelligentCollision(CubeObj* i, CubeObj* m[][maxHeight][maxDepth], int cX, int cY, int cZ) {
    // First, check collision on all immediate directions
    // on X axis...
    for (int a = -2; a<3; a++) {
    Collision::checkAndBounce(i,m[cX+a][cY][cZ]);
    }
    // ...Y axis...
    for (int b = -2; b<3; b++) {
    Collision::checkAndBounce(i,m[cX][cY+b][cZ]);
    }
    // ...and Z axis
    for (int c = -2; c<3; c++) {
    Collision::checkAndBounce(i,m[cX][cY][cZ+c]);
    }
    // Then check the diagonals too
    // (this technique is a bit wasteful here, will clean up later if I have time)
    for (int a = -2; a<3; a++) {
    for (int b = -2; b<3; b++) {
    for (int c = -2; c<3; c++) {
    Collision::checkAndBounce(i,m[cX+a][cY+b][cZ+c]);
    } } }
}

// Put a cube in the collision map
void addToCollisionMap(CubeObj* c1, CubeObj* map[][maxHeight][maxDepth]) {
  int cX = getCollisionMapSlot(c1,0);
  int cY = getCollisionMapSlot(c1,1);
  int cZ = getCollisionMapSlot(c1,2);
  map[cX][cY][cZ] = c1;
}

void findNeighbors(CubeObj* c1, CubeObj* map[][maxHeight][maxDepth]) {
  int cX = getCollisionMapSlot(c1,0);
  int cY = getCollisionMapSlot(c1,1);
  int cZ = getCollisionMapSlot(c1,2);
  // The top/bot order on these might be wrong, but it shouldn't really matter too much
  // since used to check if surrounded on a plane anyways
  c1->setNeighbors(
    map[cX+1][cY][cZ] != NULL,
    map[cX-1][cY][cZ] != NULL,
    map[cX][cY+1][cZ] != NULL,
    map[cX][cY-1][cZ] != NULL,
    map[cX][cY][cZ+1] != NULL,
    map[cX][cY][cZ-1] != NULL
  );
}

// pass cube and dimension to get map slot
int getCollisionMapSlot(CubeObj* c, int d) {
  int map = (d==0? currentMapWidth : d==1? currentMapHeight : currentMapDepth);
  return (c->get(d) - c->getSize(d)/2 + map/2*tileSize)/tileSize;
}

// Returns gameplay state
CubiorObj* getPlayer() { return &cubior[0]; }
CubiorObj* getPlayer(int i) { return &cubior[i]; }
CubeObj* getCube() { return &cube[0]; }
CubeObj* getCube(int i) { return &cube[i]; }
GoalObj* getGoal() { return &goal; }
const int getCubiorCount() { return cubiorCount; }
bool getCubiorPlayable(int i) { return cubiorPlayable[i]; }
void setCubiorPlayable(int i, bool b) { cubiorPlayable[i] = b; }
const int getMaxCubeCount() { return maxCubeCount; }
int getCubeCount() { return cubeCount; }
CameraObj* getCamera() { return &camera[0]; }
CameraObj* getCamera(int i) { return &camera[i]; }

bool getInvincibility(int n) { return cubior[n].getInvincibility(); }
void setInvincibility(int n, bool newState) { cubior[n].setInvincibility(newState); }

int getFloor() { return floor; }
int getGravity() { return gravity; }

void  enableGoodCollision() { goodCollision = true; }
void disableGoodCollision() { goodCollision = false; }
void  stopGameplay() { gameplayRunning = false; }
void startGameplay() { gameplayRunning = true; }
bool getGameplayRunning() { return gameplayRunning; }
