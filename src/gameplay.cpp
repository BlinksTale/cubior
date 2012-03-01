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
#include <iostream>
#include <stdlib.h> // for NULL

using namespace std;

const int cubiorCount = 3;
const int cubeCount = 5;
const int tileSize = 100;
const int mapEdge = 3;
const int mapWidth = 10 + mapEdge*2;
const int mapHeight= 10 + mapEdge*2;
const int mapDepth = 10 + mapEdge*2;
CubeObj* collisionMap[mapWidth][mapHeight][mapDepth];

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
    cube[i].setPos(-200*i+500,-100,0);
    cube[i].setPermalock(true);
  }
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
     //cout << "a: " << a << ", b: " << b << ", c: " << c << "\n";
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

  cout << "GameplayLoop for " << mapWidth << "\n\n\n";
  for (int i=mapDepth-1; i>=0; i--) {
  for (int j=0; j<1; j++) {
  for (int k=0; k<mapWidth; k++) {
  cout << (collisionMap[k][j][i]) << "\t";
  }
  }
  cout << "\n";
  }
  cout << "EndGameplayLoop\n\n\n";
  // Then check collision against all other obstacles (cubes/cubiors)
  for (int i = 0; i<cubiorCount; i++) {

    int cX = cubior[i].getX()/tileSize + mapWidth/2;
    int cY = cubior[i].getY()/tileSize + mapHeight/2;
    int cZ = cubior[i].getZ()/tileSize + mapDepth/2;

    cout << "x: " << cubior[i].getX()/tileSize + mapWidth/2 << "\n";
    cout << "y: " << cubior[i].getY()/tileSize + mapHeight/2 << "\n";
    cout << "z: " << cubior[i].getZ()/tileSize + mapDepth/2 << "\n";
/*
    Collision::checkAndBounce(&cubior[i],collisionMap[cX-1][cY][cZ]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX+1][cY][cZ]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX][cY-1][cZ]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX][cY+1][cZ]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX][cY][cZ-1]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX][cY][cZ+1]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX-1][cY-1][cZ-1]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX-1][cY-1][cZ+1]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX-1][cY+1][cZ-1]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX-1][cY+1][cZ+1]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX+1][cY-1][cZ-1]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX+1][cY-1][cZ+1]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX+1][cY+1][cZ-1]);
    Collision::checkAndBounce(&cubior[i],collisionMap[cX+1][cY+1][cZ+1]);
/*
*/

    for (int a = -2; a<3; a++) {
    Collision::checkAndBounce(&cubior[i],collisionMap[cX+a][cY][cZ]);
    }
    for (int b = -2; b<3; b++) {
    Collision::checkAndBounce(&cubior[i],collisionMap[cX][cY+b][cZ]);
    }
    for (int c = -2; c<3; c++) {
    Collision::checkAndBounce(&cubior[i],collisionMap[cX][cY][cZ+c]);
    }
    // Check collision with other Cubiors
/*    for (int j = i+1; j<cubiorCount; j++) {
      Collision::checkAndBounce(&cubior[i],&cubior[j]);
    }
    // Check collision with Obstacles
    for (int j = 0; j<cubeCount; j++) {
      Collision::checkAndBounce(&cubior[i],&cube[j]);
    }
*/
  }
}

// Put a cube in the collision map
void addToCollisionMap(CubeObj* c1) {
  int cX = c1->getX()/tileSize+mapWidth/2;
  int cY = c1->getY()/tileSize+mapHeight/2;
  int cZ = c1->getZ()/tileSize+mapDepth/2;
  collisionMap[cX][cY][cZ] = c1;
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
