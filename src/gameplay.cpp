/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#include "gameplay.h"
#include "cubeObj.h"
#include "goalObj.h"
#include "cubiorObj.h"
#include "collision.h"
#include <iostream>
#include <stdlib.h> // for NULL

using namespace std;

const int cubiorCount = 3;
const int cubeCount = 9;
const int tileSize = 100;
const int mapEdge = 3;
const int mapWidth = 10 + mapEdge*2;
const int mapHeight= 10 + mapEdge*2;
const int mapDepth = 10 + mapEdge*2;
bool goodCollision = true;
CubeObj* collisionMap[mapWidth][mapHeight][mapDepth];

CubiorObj cubior[cubiorCount];
CubeObj cube[cubeCount];
GoalObj goal;
static int movementSpeed = 1;
static int jumpSpeedRatio = 5;
static int rotationSpeed = 10;
int maxJump = 25;
int maxSpeed = 20;
int friction = 1;

int gravity = 2;
int floor = -100;

void gameplayStart() {
  // Cubior Start States!
  for (int i=0; i<cubiorCount; i++) {
    cubior[i].setPos(-200*i+0,100,0);
    cubior[i].moveX(3);
    cubior[i].moveY(3);
    cubior[i].moveZ(3);
    cubior[i].setHappiness(1.0-i*1.0/cubiorCount);
  }
  // and Cube Obstacle start states
  for (int i=0; i<cubeCount; i++) {
    cube[i].setPos(-100*i+00,-100,0);
    cube[i].setPermalock(true);
  }
  cube[cubeCount-4].setPos(-100*0,-000,000);
  cube[cubeCount-3].setPos(-100*2,-100,100);
  cube[cubeCount-2].setPos(-100*1,-100,100);
  cube[cubeCount-1].setPos(-100*0,-100,100);
  // Then the goal
  goal.setPos(500,200,-500);
}

// FIXME: This causes lots of lag right now. Intended to keep player inside game though
void keepInBounds(CubeObj* c1) {
    // The bounds are one fewer than the size of the grid
    // this makes checking collision in all directions from a cube never go out of bounds
    if (c1->getX()< (-mapWidth/2+mapEdge)*tileSize){c1->setX((-mapWidth/2 +mapEdge)*tileSize);}
    if (c1->getX()>=( mapWidth/2-mapEdge)*tileSize){c1->setX(( mapWidth/2 -mapEdge)*tileSize);}
    if (c1->getY()<-(mapHeight/2+mapEdge)*tileSize){c1->setY((-mapHeight/2+mapEdge)*tileSize);}
    if (c1->getY()>=(mapHeight/2-mapEdge)*tileSize){c1->setY(( mapHeight/2-mapEdge)*tileSize);}
    if (c1->getZ()< (-mapDepth/2+mapEdge)*tileSize){c1->setZ((-mapDepth/2 +mapEdge)*tileSize);}
    if (c1->getZ()>=( mapDepth/2-mapEdge)*tileSize){c1->setZ(( mapDepth/2 -mapEdge)*tileSize);}
}

void gameplayLoop() {
  // Wipe collision map, repopulate it
  for (int a=0; a<mapWidth; a++) {
  for (int b=0; b<mapHeight;b++) {
  for (int c=0; c<mapDepth; c++) {
     collisionMap[a][b][c]=0;
  } } }

  // Run main tick loop for all Cubiors...
  for (int i = 0; i<cubiorCount; i++) {
    cubior[i].tick();
    keepInBounds(&cubior[i]);
    addToCollisionMap(&cubior[i]);
  }
  // ... and all Cubes
  for (int i = 0; i<cubeCount; i++) {
    cube[i].tick();
    keepInBounds(&cube[i]);
    addToCollisionMap(&cube[i]);
  }
  // and the goal
  goal.tick();
  addToCollisionMap(&goal);

  // Then check collision against all other obstacles (cubes/cubiors)
  for (int i = 0; i<cubiorCount; i++) {

    int cX = getCollisionMapSlot(&cubior[i],0);
    int cY = getCollisionMapSlot(&cubior[i],1);
    int cZ = getCollisionMapSlot(&cubior[i],2);

    if (goodCollision) {
      explodingDiamondCollision(&cubior[i],cX,cY,cZ);
    } else {
      unintelligentCollision(&cubior[i],cX,cY,cZ);
    }
  }
}

// Takes cubior, and its slot, then checks collision
// called exploding diamond because it checks on an expanding radius
void explodingDiamondCollision(CubeObj* i, int cX, int cY, int cZ) {
  // Then check slots in relation to cXYZ
  for (int x = 0; x<mapEdge; x++) {
    for (int y = 0; y<=x; y++) {
      for (int z = 0; z<=x; z++) {
        if (x >= y+z) {
         Collision::checkAndBounce(i,collisionMap[cX+x-(y+z)][cY+y][cZ+z]);
         Collision::checkAndBounce(i,collisionMap[cX-x+(y+z)][cY+y][cZ+z]);
         Collision::checkAndBounce(i,collisionMap[cX+x-(y+z)][cY-y][cZ+z]);
         Collision::checkAndBounce(i,collisionMap[cX-x+(y+z)][cY-y][cZ+z]);
         Collision::checkAndBounce(i,collisionMap[cX+x-(y+z)][cY+y][cZ-z]);
         Collision::checkAndBounce(i,collisionMap[cX-x+(y+z)][cY+y][cZ-z]);
         Collision::checkAndBounce(i,collisionMap[cX+x-(y+z)][cY-y][cZ-z]);
         Collision::checkAndBounce(i,collisionMap[cX-x+(y+z)][cY-y][cZ-z]);
        }
      }
    }
  }
}

// Takes cubior, and its slot, then checks collision
// called unintelligent because it checks by array slot, not distance
void unintelligentCollision(CubeObj* i, int cX, int cY, int cZ) {
    // First, check collision on all immediate directions
    // on X axis...
    for (int a = -2; a<3; a++) {
    Collision::checkAndBounce(i,collisionMap[cX+a][cY][cZ]);
    }
    // ...Y axis...
    for (int b = -2; b<3; b++) {
    Collision::checkAndBounce(i,collisionMap[cX][cY+b][cZ]);
    }
    // ...and Z axis
    for (int c = -2; c<3; c++) {
    Collision::checkAndBounce(i,collisionMap[cX][cY][cZ+c]);
    }
    // Then check the diagonals too
    // (this technique is a bit wasteful here, will clean up later if I have time)
    for (int a = -2; a<3; a++) {
    for (int b = -2; b<3; b++) {
    for (int c = -2; c<3; c++) {
    Collision::checkAndBounce(i,collisionMap[cX+a][cY+b][cZ+c]);
    } } }
}

// Put a cube in the collision map
void addToCollisionMap(CubeObj* c1) {
  int cX = getCollisionMapSlot(c1,0);
  int cY = getCollisionMapSlot(c1,1);
  int cZ = getCollisionMapSlot(c1,2);
  collisionMap[cX][cY][cZ] = c1;
}

// pass cube and dimension to get map slot
int getCollisionMapSlot(CubeObj* c, int d) {
  int map = (d==0? mapWidth : d==1? mapHeight : mapDepth);
  return (c->get(d) - c->getSize(d)/2 + map/2*tileSize)/tileSize;
}

// Returns gameplay state
CubiorObj* getPlayer() { return &cubior[0]; }
CubiorObj* getPlayer(int i) { return &cubior[i]; }
CubeObj* getCube() { return &cube[0]; }
CubeObj* getCube(int i) { return &cube[i]; }
GoalObj* getGoal() { return &goal; }
int getCubiorCount() { return cubiorCount; }
int getCubeCount() { return cubeCount; }

bool getInvincibility(int n) { return cubior[n].getInvincibility(); }
void setInvincibility(int n, bool newState) { cubior[n].setInvincibility(newState); }

int getFloor() { return floor; }
int getGravity() { return gravity; }

void  enableGoodCollision() { goodCollision = true; }
void disableGoodCollision() { goodCollision = false; }
