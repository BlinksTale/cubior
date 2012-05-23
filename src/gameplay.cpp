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
#include "flatRender.h"
#include <iostream>
#include <cstdio>
#include <stdlib.h> // for NULL
#include <string> // for loading a level by var passed

using namespace std;

bool cubiorPlayable[cubiorCount];
bool goodCollision = true;
CubeObj* collisionMap[maxWidth][maxHeight][maxDepth];
CubeObj* permanentMap[maxWidth][maxHeight][maxDepth];
CubeObj cameraCube;
Map* levelMap;

int currentMapWidth;
int currentMapHeight;
int currentMapDepth;
int cubeCount;
int currentLevel = 0;
bool changeLevel = false;

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

// Changing game state variables
bool gameplayRunning = true;

// Quick math function for keepInBounds
int getEdge(int dimension, int neg) {
  return (neg*(dimension-(neg>0)*1)/2)*tileSize-neg*1;
}
// FIXME: This causes lots of lag right now. Intended to keep player inside game though
void keepInBounds(CubeObj* c1) {
    // The bounds are one fewer than the size of the grid
    // this makes checking collision in all directions from a cube never go out of bounds
    if (c1->getX()< getEdge(currentMapWidth,-1)){c1->setX(getEdge(currentMapWidth,-1));}
    if (c1->getX()>=getEdge(currentMapWidth,1)){c1->setX(getEdge(currentMapWidth,1));}
    if (c1->getY()<getEdge(currentMapHeight,-1)){c1->setY(getEdge(currentMapHeight,-1));}
    if (c1->getY()>=getEdge(currentMapHeight,1)){c1->setY(getEdge(currentMapHeight,1));}
    if (c1->getZ()< getEdge(currentMapDepth,-1)){c1->setZ(getEdge(currentMapDepth,-1));}
    if (c1->getZ()>=getEdge(currentMapDepth,1)){c1->setZ(getEdge(currentMapDepth,1));}
}

// Checks if any side of a cube is on the edge of the map
void findEdges(CubeObj* c1, CubeObj* map[][maxHeight][maxDepth]) {
  int cX = getCollisionMapSlot(c1,0);
  int cY = getCollisionMapSlot(c1,1);
  int cZ = getCollisionMapSlot(c1,2);
  c1->setEdges(
    c1->getX() >=(currentMapWidth/2-1)*tileSize,
    c1->getX() <=-(currentMapWidth/2-1)*tileSize,
    c1->getY() >=(currentMapHeight/2-1)*tileSize,
    c1->getY() <=-(currentMapHeight/2-1)*tileSize,
    c1->getZ() >=(currentMapDepth/2-1)*tileSize,
    c1->getZ() <=-(currentMapDepth/2-1)*tileSize
  );
}

// Put a Cubior back in its start spot
void resetCubior(int i) {
  // Put camera in drop down spot
  camera[i].resetPos();
  // Put cubior in falling spot
	int distFromCenter = (i+1)/2;
	int directionFromCenter = 1+(i%2)*(-2);
  cubior[i].setPos(-200*(distFromCenter*directionFromCenter),100, currentMapWidth*tileSize*1/2-400);
  cubior[i].setMomentumX(0);
  cubior[i].setMomentumY(0);
  cubior[i].setMomentumZ(0);
  cubior[i].moveX(0);
  cubior[i].moveY(3);
  cubior[i].moveZ(3);
}

// Load in a level and set it up
void gameplayStart(string levelToLoad) {
  if (gameplayRunning) {

    // First wipe the current map
    wipeCurrentMap(permanentMap);

    // Then read in a new map
    levelMap = MapReader::readMap(levelToLoad);
    currentMapWidth = levelMap->getWidth();
    currentMapHeight= levelMap->getHeight();
    currentMapDepth = levelMap->getDepth();
    cubeCount = levelMap->getCubeCount();
    if (currentMapWidth > playableWidth) { currentMapWidth = playableWidth; }
    if (currentMapHeight> playableHeight){ currentMapHeight= playableHeight;}
    if (currentMapDepth > playableDepth) { currentMapDepth = playableDepth; }
    if (cubeCount > maxCubeCount) { cubeCount = maxCubeCount; }

    // Setup player positions and cameras
    for (int i=0; i<cubiorCount; i++) {
      // Starting camera and player pos
      resetCubior(i);
  
      // Start camera!
	    camera[i].resetPos();
      camera[i].alwaysFollow(&cubior[i],&goal);

      // Cubior Start State
      cubior[i].setHappiness(1.0-i*1.0/cubiorCount);
    }

    // Then ensure at least P1 is playing
    cubiorPlayable[0] = true;

    // and Cube Obstacle start states
    for (int i=0; i<cubeCount; i++) {
      cube[i].setPermalock(true);
    }
            
    // Load cubes in from level reader
    int currentCube = 0;
    for (int z=0; z<levelMap->getDepth(); z++) {
      for (int x=0; x<levelMap->getWidth(); x++) {
        for (int y=0; y<levelMap->getHeight(); y++) {
          if (levelMap->getCubeAt(x,y,z) != 0 && currentCube < cubeCount) {
            cube[currentCube].setPos(tileSize*(x-levelMap->getWidth()/2),tileSize*(y-levelMap->getHeight()/2),tileSize*(z-levelMap->getDepth()/2));
            cube[currentCube].setMaterial(levelMap->getCubeAt(x,y,z)->getMaterial());
            currentCube++;
          }
        }
      }
    }
    
    // Then the goal
    goal.setPos(000,levelMap->getGoalHeight(),levelMap->getGoalDepth());

    // Then populate permamap
    // ... with permanent Cubes
    for (int i = 0; i<cubeCount; i++) {
      cube[i].tick();
      //keepInBounds(&cube[i]);
      addToCollisionMap(&cube[i], permanentMap);
    }
    // Then set their neighbors, for more efficient rendering
    for (int i = 0; i<cubeCount; i++) {
      findNeighbors(&cube[i], permanentMap);
      findEdges(&cube[i], permanentMap);
    }

  }
}

// To load the next level
void nextLevel() {
  // Set next level number
  changeLevel = true;
  currentLevel = (currentLevel + 1) % totalLevels;

  // Then load the appropriate level
  int n;
  char buffer[100];
  n=sprintf(buffer, "./maps/cubiorMap%i.cubior", currentLevel);
  gameplayStart(buffer);
  initVisuals();
}

void gameplayLoop() {
  if (gameplayRunning) {
	  
	  // Only recognize a level change for one loop
	  if (changeLevel) { changeLevel = false; }
	  
    wipeCurrentMap(collisionMap);
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
        /*if (i == 0) { cout << "Cubior pos:" << endl;
          cout << "x is " << cubior[i].getX() << ", ";
          cout << "y is " << cubior[i].getY() << ", ";
          cout << "z is " << cubior[i].getZ() << endl;
          cout << "While currentMapWidth = " << currentMapWidth << ", ";
          cout << "currentMapHeight = " << currentMapHeight << ", ";
          cout << "currentMapDepth = " << currentMapDepth << ", ";
          cout << "and tileSize is " << tileSize << endl;
        }*/
      }
    }
    
    // Finally, make camera catchup
    for (int i=0; i<cubiorCount; i++) {
  	  if (changeLevel) { break; }
	    if (cubiorPlayable[i]){
        // Basic setup
        camera[i].tick();

        // And bounce off walls if colliding
        int cX = getCollisionMapSlot(&camera[i],0);
        int cY = getCollisionMapSlot(&camera[i],1);
        int cZ = getCollisionMapSlot(&camera[i],2);
        if (goodCollision) {
          explodingDiamondCollision(&camera[i],permanentMap,cX,cY,cZ);
          explodingDiamondCollision(&camera[i],collisionMap,cX,cY,cZ);
        } else {
          unintelligentCollision(&camera[i],permanentMap,cX,cY,cZ);
          unintelligentCollision(&camera[i],collisionMap,cX,cY,cZ);
        }
      }
    }

  }
}

// Tells Camera if it can see player or not, sets up Line of Sight
void checkCameraLOS(CameraObj* c, CubeObj* m[][maxHeight][maxDepth]) {
  int cX = getCollisionMapSlot(c,0);
  int cY = getCollisionMapSlot(c,1);
  int cZ = getCollisionMapSlot(c,2);
  c->setLOS(true);
}

// Takes cubior, and its slot, then checks collision
// called exploding diamond because it checks on an expanding radius
void explodingDiamondCollision(CubeObj* i, CubeObj* m[][maxHeight][maxDepth], int cX, int cY, int cZ) {
  // Then check slots in relation to cXYZ
  for (int x = 0; x<mapEdge; x++) {
    for (int y = 0; y<=x; y++) {
      for (int z = 0; z<=x; z++) {
        if (x >= y+z) {
			/*
			 * YUCK! Lots of ugly breaks in case one collision is a levelChange.
			 * TODO: Fix so that a flag is set when goal collided with, then act
			 *       on it after the current loop ends.
			 */
			if (changeLevel) { break; }
			Collision::checkAndBounce(i,m[cX+x-(y+z)][cY+y][cZ+z]);
			if (changeLevel) { break; }
			Collision::checkAndBounce(i,m[cX-x+(y+z)][cY+y][cZ+z]);
			if (changeLevel) { break; }
			Collision::checkAndBounce(i,m[cX+x-(y+z)][cY-y][cZ+z]);
			if (changeLevel) { break; }
			Collision::checkAndBounce(i,m[cX-x+(y+z)][cY-y][cZ+z]);
			if (changeLevel) { break; }
			Collision::checkAndBounce(i,m[cX+x-(y+z)][cY+y][cZ-z]);
			if (changeLevel) { break; }
			Collision::checkAndBounce(i,m[cX-x+(y+z)][cY+y][cZ-z]);
			if (changeLevel) { break; }
			Collision::checkAndBounce(i,m[cX+x-(y+z)][cY-y][cZ-z]);
			if (changeLevel) { break; }
			Collision::checkAndBounce(i,m[cX-x+(y+z)][cY-y][cZ-z]);
			if (changeLevel) { break; }
		}
		  if (changeLevel) { break; }
	  }
		if (changeLevel) { break; }
	}
	  if (changeLevel) { break; }
  }}

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
    map[cX+1][cY][cZ] != 0,
    map[cX-1][cY][cZ] != 0,
    map[cX][cY+1][cZ] != 0,
    map[cX][cY-1][cZ] != 0,
    map[cX][cY][cZ+1] != 0,
    map[cX][cY][cZ-1] != 0
  );
}

// Wipe current collision map to prep for repopulating it
void wipeCurrentMap(CubeObj* map[][maxHeight][maxDepth]){
  for (int a=0; a<currentMapWidth; a++) {
  for (int b=0; b<currentMapHeight;b++) {
  for (int c=0; c<currentMapDepth; c++) {
     map[a][b][c] = NULL;
  } } }
}

// Wipe the full collision map to prep for repopulating it
void wipeFullMap(CubeObj* map[][maxHeight][maxDepth]){
  for (int a=0; a<maxWidth; a++) {
  for (int b=0; b<maxHeight;b++) {
  for (int c=0; c<maxDepth; c++) {
     map[a][b][c] = NULL;
  } } }
}

// pass cube and dimension to get map slot
int getCollisionMapSlot(CubeObj* c, int d) {
  int map = (d==0? currentMapWidth : d==1? currentMapHeight : currentMapDepth);
  int cD = c->get(d);
  int cS = 50;//(c->getSize(d))/2; // FIXME: USING C->GETSIZE(D) HERE CAUSES A SEGFAULT >:( probably has to do with virtual functions
  int mS = map/2*tileSize;
  int result = (cD - cS + mS)/tileSize;
  return result;
}

// Returns gameplay state
CubiorObj* getPlayer() { return &cubior[0]; }
CubiorObj* getPlayer(int i) { return &cubior[i]; }
CubeObj* getCube() { return &cube[0]; }
CubeObj* getCube(int i) { return &cube[i]; }
GoalObj* getGoal() { return &goal; }
const int getCubiorCount() { return cubiorCount; }
bool getCubiorPlayable(int i) { return cubiorPlayable[i]; }
void setCubiorPlayable(int i, bool b) {
  resetCubior(i);
  cubiorPlayable[i] = b;
}
const int getMaxCubeCount() { return maxCubeCount; }
int getCubeCount() { return cubeCount; }
CameraObj* getCamera() { return &camera[0]; }
CameraObj* getCamera(int i) { return &camera[i]; }

bool getInvincibility(int n) { return cubior[n].getInvincibility(); }
void setInvincibility(int n, bool newState) { cubior[n].setInvincibility(newState); }

int getGravity() { return gravity; }

void  enableGoodCollision() { goodCollision = true; }
void disableGoodCollision() { goodCollision = false; }
void  stopGameplay() { gameplayRunning = false; }
void startGameplay() { gameplayRunning = true; }
bool getGameplayRunning() { return gameplayRunning; }
float getMapRed()   { return levelMap->getRed(); }
float getMapGreen() { return levelMap->getGreen(); }
float getMapBlue()  { return levelMap->getBlue(); }

