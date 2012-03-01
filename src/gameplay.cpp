/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#include "gameplay.h"
#include "cubeObj.h"
#include "cubiorObj.h"
#include "collision.h"
#include <stdlib.h> // for NULL

using namespace std;

const int cubiorCount = 3;
const int cubeCount = 5;
const int tileSize = 100;
const int mapWidth = 10;
const int mapHeight = 10;
const int mapDepth = 10;
CubeObj* collisionMap[mapWidth/tileSize][mapHeight/tileSize][mapDepth/tileSize];

CubiorObj cubior[cubiorCount];
CubeObj cube[cubeCount];
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
    cubior[i].setPos(-200*i+0,0,0);
    cubior[i].moveX(3);
    cubior[i].moveY(3);
    cubior[i].moveZ(3);
    cubior[i].setHappiness(1.0-i*1.0/cubiorCount);
  }
  // and Cube Obstacle start states
  for (int i=0; i<cubeCount; i++) {
    cube[i].setPos(-100*i+0,-100,0);
    cube[i].setPermalock(true);
  }
}

// FIXME: This causes lots of lag right now. Intended to keep player inside game though
void keepInBounds(CubeObj* c1) {
    if (c1->getX() < -mapWidth/2 * tileSize) { c1->setX(-mapWidth/2 * tileSize);}
    if (c1->getX() >= mapWidth/2 * tileSize) { c1->setX( mapWidth/2 * tileSize);}
    if (c1->getY() <-mapHeight/2 * tileSize) { c1->setY(-mapHeight/2* tileSize);}
    if (c1->getY() >=mapHeight/2 * tileSize) { c1->setY( mapHeight/2* tileSize);}
    if (c1->getZ() < -mapDepth/2 * tileSize) { c1->setZ(-mapDepth/2 * tileSize);}
    if (c1->getZ() >= mapDepth/2 * tileSize) { c1->setZ( mapDepth/2 * tileSize);}
}

void gameplayLoop() {
  // Wipe collision map, repopulate it
  for (int a=0; a<mapWidth/tileSize; a++) {
  for (int b=0; b<mapHeight/tileSize;b++) {
  for (int c=0; c<mapDepth/tileSize; c++) {
     collisionMap[a][b][c]=NULL;
  } } }

  // Run main tick loop for all Cubiors and Cubes
  for (int i = 0; i<cubiorCount; i++) {
    cubior[i].tick();
    keepInBounds(&cubior[i]);
    collisionMap[cubior[i].getX()/tileSize][cubior[i].getY()/tileSize][cubior[i].getZ()/tileSize] = &cubior[i];
  }
  for (int i = 0; i<cubeCount; i++) {
    cube[i].tick();
    keepInBounds(&cube[i]);
    collisionMap[cube[i].getX()/tileSize][cube[i].getY()/tileSize][cube[i].getZ()/tileSize] = &cube[i];
  }
  for (int i = 0; i<cubiorCount; i++) {
    int diffX = 0;
    int diffY = 0;
    int diffZ = 0;

    // Check collision with other Cubiors
    for (int j = i+1; j<cubiorCount; j++) {
      if (Collision::between(&cubior[i],&cubior[j])) {
        //diffX += Collision::getDiff(&cubior[i],&cubior[j],0);
        //diffY += Collision::getDiff(&cubior[i],&cubior[j],1);
        //diffZ += Collision::getDiff(&cubior[i],&cubior[j],2);
        Collision::bounce(&cubior[i],&cubior[j]);
        //Collision::balanceMomentum(&cubior[i],&cubior[j]);
      }
    }
    // Check collision with Obstacles
    for (int j = 0; j<cubeCount; j++) {
      if (Collision::between(&cubior[i],&cube[j])) {
        //diffX += Collision::getDiff(&cubior[i],&cube[j],0);
        //diffY += Collision::getDiff(&cubior[i],&cube[j],1);
        //diffZ += Collision::getDiff(&cubior[i],&cube[j],2);
        Collision::bounce(&cubior[i],&cube[j]);
        //Collision::balanceMomentum(&cubior[i],&cube[0]);
      }
    }
    
    //Collision::bounceMeByDiff(&cubior[i],diffX,diffY,diffZ);
  }
}

// Returns gameplay state
CubiorObj* getPlayer() { return &cubior[0]; }
CubiorObj* getPlayer(int i) { return &cubior[i]; }
CubeObj* getCube() { return &cube[0]; }
CubeObj* getCube(int i) { return &cube[i]; }
int getCubiorCount() { return cubiorCount; }
int getCubeCount() { return cubeCount; }

bool getInvincibility(int n) { return cubior[n].getInvincibility(); }
void setInvincibility(int n, bool newState) { cubior[n].setInvincibility(newState); }

int getFloor() { return floor; }
int getGravity() { return gravity; }
