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
#include <cmath> // for trig stuff with getAngleBetween
#include <iostream>
#include <cstdio>
#include <stdlib.h> // for NULL
#include <string> // for loading a level by var passed

using namespace std;

bool cubiorPlayable[cubiorCount];
bool goodCollision = true;
CubeObj* collisionMap[maxWidth][maxHeight][maxDepth];
CubeObj* permanentMap[maxWidth][maxHeight][maxDepth];
CubeObj cameraCube; // FIXME: Seems to only be here for collision purposes
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
int maxCameraHeight = 2000;
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
  // And reset its visible-yet-intended count
  camera[i].setVisibleIntended(0);
  
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
        cameraCube.setPos(camera[i].getX(),camera[i].getY(),camera[i].getZ());
        // using cameraCube here since a lack thereof make camera's collision stop working 
        int cX = getCollisionMapSlot(&cameraCube,0);
        int cY = getCollisionMapSlot(&cameraCube,1);
        int cZ = getCollisionMapSlot(&cameraCube,2);
        if (goodCollision) {
          explodingDiamondCollision(&cameraCube,permanentMap,cX,cY,cZ);
          explodingDiamondCollision(&cameraCube,collisionMap,cX,cY,cZ);
        } else {
          unintelligentCollision(&cameraCube,permanentMap,cX,cY,cZ);
          unintelligentCollision(&cameraCube,collisionMap,cX,cY,cZ);
        }
        camera[i].setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());

        //cout << "Check visibility"<<endl;
        ensurePlayerVisible(i);
        }
    }

  }
}

// This checks if the player is visible, and fixes invisible cases too
void ensurePlayerVisible(int i) {
  //FIXME: Only need this cout for understanding range of angles or how they work
  cout << "baseAngle: " <<  getAngleBetween(camera[i].getX(),camera[i].getZ(), camera[i].getPermanentTarget()->getX(), camera[i].getPermanentTarget()->getZ()) << endl;
  // Don't check if zooming in or too close
  if (camera[i].heightToPlayer() < maxCameraHeight && camera[i].distToPlayer() > tileSize/2) {
    // Otherwise,
    if (playerVisible(i)) {
      //cout << "Player visible" << endl;
      //cout << "with camera saying " << (camera[i].getLOS() ? "true" : "false") << endl;
      //cout << "and dist to player " << camera[i].distToPlayer() << endl;
      
      //If visible, make sure you're not moving to a new angle anymore!
      // then first, make absolutely sure you still need an intended pos
      /*if (camera[i].getFoundIntendedPos()) {
        if (camera[i].getVisibleIntended() > visibleIntendedMax) {
          camera[i].setFoundIntendedPos(false);
        } else {
          camera[i].setVisibleIntended(camera[i].getVisibleIntended()+1);
        }
      } else if (camera[i].getVisibleIntended() > 0) {
        camera[i].setVisibleIntended(0);
      }*/
      // Well that didn't work as planned...
      
      // Attempt #2! Stop moving camera once player is visible
      if (camera[i].getFoundIntendedPos()) {
        camera[i].setBackupFreedom(false); // Better not backup if you found the player
      }
    } else {
      // and fix if needed
      //cout << "Player hidden!" << endl;
      fixPlayerVisibility(i);
    }
    //cout << "Camera " << i << "'s vis = " << camera[i].getLOS() << " & p1 to goal = " << camera[i].goalWithinNearRange() << endl;
  }
}

// Gives player non-visibility
bool playerVisible(int i) {
  // Must check LOS first, or getLOS will not be updated
  checkCameraLOS(&camera[i],permanentMap);
  // then return the newly updated results
  return  camera[i].getLOS();
}

// We know the player is not visible, so fix it!
void fixPlayerVisibility(int i) {
  if (rotateIfInvisible) {
    if (!camera[i].getFoundIntendedPos()) {
      rotateToPlayer(i);
    }
  } else {
    moveToPlayer(i);
  }
  //cout << "Still here, los is " << camera[i].getLOS() << " with cam at (" << camera[i].getX() << ", " << camera[i].getY() << ", " << camera[i].getZ() << ") aiming for (" << camera[i].getPermanentTarget()->getX() << ", " << camera[i].getPermanentTarget()->getY() << ", " << camera[i].getPermanentTarget()->getZ() << ")" << endl;

}

// Get the camera closer to the player
void moveToPlayer(int i) {
  // go forwards until player is visible
  while (!playerVisible(i)) {
    cameraCube.setPos(camera[i].getX(),camera[i].getY(),camera[i].getZ());
    cameraCube.changePosTowards(camera[i].getPermanentTarget(),tileSize/2.0);
    camera[i].setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
  }
}

// Try to find an angle & rotate the camera to angle to see the player
void rotateToPlayer(int i) {
  
  bool foundAnAngle = false;
  CubeObj intendedPos;
  bool foundIntendedPos = false;
  CubeObj oldCamera;
  
  // Pivot point for rotation
  int targetX = camera[i].getPermanentTarget()->getX();
  int targetZ = camera[i].getPermanentTarget()->getZ();
  // Position we will move to on each turn
  int oldX = camera[i].getX();
  int oldY = camera[i].getY();
  int oldZ = camera[i].getZ();
  int newX = oldX;
  int newZ = oldZ;
  oldCamera.setPos(oldX,oldY,oldZ);
  
  // Hypotenuse for triangle formed between camera and target
  int hyp = camera[i].groundDistToPlayer();
  //cout << "GroundDistToPlayer: " << hyp << endl;
  //cout << "our own math for it " << (sqrt(pow(oldX-targetX,2)+pow(oldZ-targetZ,2))) << endl;
  
  // Angle that we will be moving away from, pivot point side
  float baseAngle = /*M_PI*3/2.0 - */ getAngleBetween(camera[i].getX(),camera[i].getZ(),targetX,targetZ);
  // Angle we will be moving to, based on pivot
  float newAngle = baseAngle;
  cout << "oldX: " << oldX << ", oldX: " << oldZ << endl;
  cout << "targetX: " << targetX << ", targetZ: " << targetZ << endl;
  cout << "baseAngle, " << baseAngle << ", newAngle, " << newAngle << endl;
  
  //cout << "START" << endl;
  // rotate until player is visible or 180 from start
  for (int k = 0; k<anglesToTry; k++)
  {
    // New pivot angle to go to
    newAngle = baseAngle + (M_PI*2.0/anglesToTry)*k/2.0*(1.0-2.0*(k%2));
    
    cout << "newAngle, " << newAngle << endl;
    // Set new intended pos for each turn
    newX = targetX + hyp*sin(newAngle);
    newZ = targetZ + hyp*cos(newAngle);
    cout << "newX: " << newX << ", newZ: " << newZ << endl;
    
    //cout << "Target " << targetX << ", " << targetZ << endl;
    //cout << "hyp is " << hyp << " w/ newX " << newX << " and newZ " << newZ << endl;
    //cout << "newAngle is " << newAngle << " from baseAngle " << baseAngle << endl;
    
    // Leaving camera cube in here just in case forgetting it would ruin something, not sure
    // FIXME later by testing it with no camera cube step
    cameraCube.setPos(newX,camera[i].getY(),newZ);
    camera[i].setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
    
    // Once you see the player visible, remember it! If a clear shot to 
    // the camera, use the position for sure.
    // May need to fix this later, as requires setting cam pos every time
    if (playerVisible(i)) {
      camera[i].tick();
      foundAnAngle = true;
      //cout << "Found a fix! (might not be perfect)" << endl;
           
      // Next, test if it is also a straight shot from the current camera
      if (checkPathVisibility(&cameraCube,&oldCamera, permanentMap)) {

        //cout << "Actually, it is perfect!" << endl;
        // Found a straight shot? Remember it and stop looking for anything better!
        intendedPos.setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
        foundIntendedPos = true;
        break;

      }

      // Otherwise, if your first result, save it so we have *something*
      if (!foundIntendedPos) {

        //cout << "Not perfect, but a good first fix!" << endl;
        intendedPos.setPos(camera[i].getX(),camera[i].getY(),camera[i].getZ());

        // And now we have found such a position, so remember that it's taken
        foundIntendedPos = true;

      }      
    }
    //cout << "No perfect fix yet on try " << k << " of " << anglesToTry << endl;
  }
  //cout << "END OF ATTEMPTS" << endl;
  
  //if (!foundAnAngle) {
    //cout << "No good results" << endl;
    // And if no escape, just go back to what we had :/
    cameraCube.setPos(oldX,oldY,oldZ);
    camera[i].setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
  //}
  
  // Otherwise, the best we found is the best we found, use it!
  if (foundIntendedPos) {
    //cout << "Got something!" << endl;
    //FIXME: commented out since seemingly pointless.
    // cameraCube.setPos(intendedPos.getX(),intendedPos.getY(),intendedPos.getZ());
    // Meanwhile, this is commented out to use intendedPos instead
    //camera[i].setPos(intendedPos.getX(),intendedPos.getY(),intendedPos.getZ());
    
    camera[i].setIntendedPos(&intendedPos);
  }
  cout << "foundIntendedPos = " << (foundIntendedPos ? "true" : "false") << endl;
}

bool checkSlotPathVisibility(int aX, int aY, int aZ, int gX, int gY, int gZ, CubeObj* m[][maxHeight][maxDepth]) {
  
  bool showData = false;
  int cX = aX, cY = aY, cZ = aZ;
  
  // Tracker moves along the line, getting as close as possible
  CubeObj tracker;
  tracker.setPos(slotToPosition(cX,0), slotToPosition(cY,1), slotToPosition(cZ,2));
  
  if (showData) {
    cout << "STAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAART!" << endl;
    cout << "goal @ <" << (gX) << ", " << (gY) << ", " << (gZ) << ">" << endl;
    cout << "were @ <" << tracker.getX() << ", " << tracker.getY() << ", " << tracker.getZ() << ">" << endl;
    cout << "Trying to reach [" << gX << ", " << gY << ", " << gZ << "]" << endl;
    cout << "Currently viewing [" << cX << ", " << cY << ", " << cZ << "]" << endl;
  }
  
  // While not arrived, search
  while(cX != gX || cY != gY || cZ != gZ) {
    
    // Found something there? Make sure it has 2D of neighbors (is a real threat)
    if (m[cX][cY][cZ] != NULL && m[cX][cY][cZ]->isWall()) {
      if (showData) {
        cout << "!!!!!!!!!!!!!!!!!!! NOT VISIBLE !!!!!!!!!!!!!!!" << endl;
        cout << "!!!!!!!!!!!!!!!!!!! NOT VISIBLE !!!!!!!!!!!!!!!" << endl;
        cout << "!!!!!!!!!!!!!!!!!!! NOT VISIBLE !!!!!!!!!!!!!!!" << endl;
        cout << "!!!!!!!!!!!!!!!!!!! NOT VISIBLE !!!!!!!!!!!!!!!" << endl;
        cout << "!!!!!!!!!!!!!!!!!!! NOT VISIBLE !!!!!!!!!!!!!!!" << endl;
        cout << "!!!!!!!!!!!!!!!!!!! NOT VISIBLE !!!!!!!!!!!!!!!" << endl;
        cout << "!!!!!!!!!!!!!!!!!!! NOT VISIBLE !!!!!!!!!!!!!!!" << endl;
        cout << "Not visible at <" << tracker.getX() << ", " << tracker.getY() << ", " << tracker.getZ() << ">" << endl;
        cout << "Not visible at [" << cX << ", " << cY << ", " << cZ << "]" << endl;
      }
      
      return false;
    }
    // Otherwise, move closer
    tracker.changePosTowards(slotToPosition(gX,0),slotToPosition(gY,1),slotToPosition(gZ,2),tileSize/16.0); // was /4

    // Then Reset Cam Tracker Pos
    cX = getCollisionMapSlot(&tracker,0);
    cY = getCollisionMapSlot(&tracker,1);
    cZ = getCollisionMapSlot(&tracker,2);
    
    if (showData) {
      cout << "<" << tracker.getX() << ", " << tracker.getY() << ", " << tracker.getZ() << ">, ";
      cout << "[" << cX << ", " << cY << ", " << cZ << "] -> ";
      cout << "[" << gX << ", " << gY << ", " << gZ << "]" << endl;
    }
    
    //cout << "cX is " << cX << ", cY is " << cY << ", cZ is " << cZ << endl;
    //cout << "gX is " << gX << ", gY is " << gY << ", gZ is " << gZ << endl;
    //cout << "From (" << tracker.getX() << ", " << tracker.getY() << ", " << tracker.getZ() << ") to (" << c->getPermanentTarget()->getX() << ", " << c->getPermanentTarget()->getY() << ", " << c->getPermanentTarget()->getZ() << ")" << endl;
  }
  return true;
}

bool checkPathVisibility(CubeObj* c2, CubeObj* target, CubeObj* m[][maxHeight][maxDepth]) {
  // Used to check all spaces between cam and target,
  // it will follow a line from the cameraObj to the player within the perm map
  // checking each slot along that line, and returning false if there is an
  // occupation before the player is reached.

  //cout << "camera currently at " << (c2->getX()) << ", " << (c2->getY()) << ", " << (c2->getZ()) << endl;
  // Cam Tracker Pos
  // will check out all the spots before trying to move into them
  int cX, cY, cZ; 
  cX = getCollisionMapSlot(c2,0);
  cY = getCollisionMapSlot(c2,1);
  cZ = getCollisionMapSlot(c2,2);
  // Goal Pos
  // where the player is, and we want to go eventually
  int gX = getCollisionMapSlot(target,0);
  int gY = getCollisionMapSlot(target,1);
  int gZ = getCollisionMapSlot(target,2);
  //cout << "Checking slots visibility " << cX << ", " << cY << ", " << cZ << " against " << gX << ", " << gY << ", " << gZ << endl;
  return checkSlotPathVisibility(cX,cY,cZ,gX,gY,gZ,m);
}

// Tells Camera if it can see player or not, sets up Line of Sight
void checkCameraLOS(CameraObj* c, CubeObj* m[][maxHeight][maxDepth]) {

  // Used to check all spaces between cam and target,
  // it will follow a line from the cameraObj to the player within the perm map
  // checking each slot along that line, and returning false if there is an
  // occupation before the player is reached.

  /*cout << "matched with [" << gX << ", " << gY << ", " << gZ << "]" << endl;
  cout << "THUS IT MUST BE SUCH THAT IT IS VISIBLE" << endl;
  */
  //cout << "camera currently at " << (c->getX()) << ", " << (c->getY()) << ", " << (c->getZ()) << endl;
  CubeObj tracker;
  tracker.setPos(c->getX(), c->getY(), c->getZ());
  // FIXME: Feeling rusty, do I need to delete tracker again to avoid a mem leak, or is containment in function ample?
  
  c->setLOS(checkPathVisibility(&tracker,c->getPermanentTarget(),m));
}

// No checkAndBounce if out of bounds
void tryCheckAndBounce(CubeObj* i, CubeObj* m[][maxHeight][maxDepth], int cX, int cY, int cZ) {
    if (cX >= 0 && cX < maxWidth && cY >= 0 && cY < maxHeight && cZ >=0 && cZ < maxDepth) {
        Collision::checkAndBounce(i,m[cX][cY][cZ]);
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
			/*
			 * YUCK! Lots of ugly breaks in case one collision is a levelChange.
			 * TODO: Fix so that a flag is set when goal collided with, then act
			 *       on it after the current loop ends.
			 */
			if (changeLevel) { break; }
			tryCheckAndBounce(i,m,cX+x-(y+z),cY+y,cZ+z);
			if (changeLevel) { break; }
			tryCheckAndBounce(i,m,cX-x+(y+z),cY+y,cZ+z);
			if (changeLevel) { break; }
			tryCheckAndBounce(i,m,cX+x-(y+z),cY-y,cZ+z);
			if (changeLevel) { break; }
			tryCheckAndBounce(i,m,cX-x+(y+z),cY-y,cZ+z);
			if (changeLevel) { break; }
			tryCheckAndBounce(i,m,cX+x-(y+z),cY+y,cZ-z);
			if (changeLevel) { break; }
			tryCheckAndBounce(i,m,cX-x+(y+z),cY+y,cZ-z);
			if (changeLevel) { break; }
			tryCheckAndBounce(i,m,cX+x-(y+z),cY-y,cZ-z);
			if (changeLevel) { break; }
			tryCheckAndBounce(i,m,cX-x+(y+z),cY-y,cZ-z);
			if (changeLevel) { break; }
		}
		  if (changeLevel) { break; }
	  }
		if (changeLevel) { break; }
	}
	  if (changeLevel) { break; }
  }
}

// Takes cubior, and its slot, then checks collision
// called unintelligent because it checks by array slot, not distance
void unintelligentCollision(CubeObj* i, CubeObj* m[][maxHeight][maxDepth], int cX, int cY, int cZ) {
    // First, check collision on all immediate directions
    // on X axis...
    for (int a = -2; a<3; a++) {
    tryCheckAndBounce(i,m,cX+a,cY,cZ);
    }
    // ...Y axis...
    for (int b = -2; b<3; b++) {
    tryCheckAndBounce(i,m,cX,cY+b,cZ);
    }
    // ...and Z axis
    for (int c = -2; c<3; c++) {
    tryCheckAndBounce(i,m,cX,cY,cZ+c);
    }
    // Then check the diagonals too
    // (this technique is a bit wasteful here, will clean up later if I have time)
    for (int a = -2; a<3; a++) {
    for (int b = -2; b<3; b++) {
    for (int c = -2; c<3; c++) {
    tryCheckAndBounce(i,m,cX+a,cY+b,cZ+c);
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
  int cubePosition = c->get(d);
  int cubeRadius = 50;//(c->getSize(d))/2; // FIXME: USING C->GETSIZE(D) HERE CAUSES A SEGFAULT >:( probably has to do with virtual functions
  int mapHalfSize = map/2*tileSize;
  int result = (cubePosition - cubeRadius + mapHalfSize)/tileSize;
  return result;
}

// pass cube and dimension to get map slot
// exact inverse of getCollisionMapSlot
int getCollisionMapPosition(int slot, int d) {
  int map = (d==0? currentMapWidth : d==1? currentMapHeight : currentMapDepth);
  int cubeRadius = 50;//(c->getSize(d))/2; // FIXME: USING C->GETSIZE(D) HERE CAUSES A SEGFAULT >:( probably has to do with virtual functions
  int mapHalfSize = map/2*tileSize;
  int result = slot*tileSize - mapHalfSize + cubeRadius;
  return result;
}

// Shorthand for getCollisionMapSlot
int positionToSlot(CubeObj* c, int d) {
  return getCollisionMapSlot(c,d);
}
// Shorthand for getCollisionMapPosition
int slotToPosition(int slot, int d) {
  return getCollisionMapPosition(slot,d);
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

// Return the angle between two points.
// Definitely in radians... but currently doing PI/2 to -PI/2, 0, PI/2 to -PI/2, 0.
// Wow, I think I finally got this working. FIXME because I want to double check
// or do a unit test or something... but man, that looks pretty good so far.
float getAngleBetween(int a, int b, int x, int y) {
  int diffX = a - x;
  int diffY = b - y;
  float result = atan(diffY*1.0/diffX) + (diffX>0)*M_PI;
  // So we deal with radians from -M_PI/2 to M_PI*3/2
  return diffX == 0 ? (diffY > 0 ? M_PI*3.0/2.0 : M_PI*1.0/2.0) : result;
}
