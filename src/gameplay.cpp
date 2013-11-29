/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#include "gameplay.h"
#include "keyboard.h"
#include "cameraObj.h"
#include "networking.h"
#include "cubeObj.h"
#include "goalObj.h"
#include "cubiorObj.h"
#include "collision.h"
#include "mapReader.h"
#include "flatRender.h"
#include "music.h"
#include "sfx.h"

#define _USE_MATH_DEFINES
#include <math.h> // for M_PI
#include <cmath> // for trig stuff with getAngleBetween
#include <iostream>
#include <cstdio>
#include <stdlib.h> // for NULL
#include <string> // for loading a level by var passed
#include <vector>

using namespace std;

bool automatedCamera = false; // (don't try to move the camera automatrically unless specified)
bool zoomingCamera = false; // whether the camera maintains a clear LOS by zooming or not
bool zoomingCameraRightNow = false;

bool cubiorPlaying[cubiorCount];// = {false,false,false,false};
bool cubiorOnline[cubiorCount];
bool goodCollision = true;
CubeObj* collisionMap[maxWidth][maxHeight][maxDepth];

//#define newPermanentMap true

//#define newPermanentMap true;
#ifdef newPermanentMap
  Map* permanentMap = new Map(maxWidth,maxHeight,maxDepth);
#else
  CubeObj* permanentMap[maxWidth][maxHeight][maxDepth];
#endif

CubeObj cameraCube; // FIXME: Seems to only be here for collision purposes
Map* levelMap;

int currentMapWidth;
int currentMapHeight;
int currentMapDepth;
int cubeCount;
int currentLevel = 0;
bool changeLevel = false;
bool firstTimeStarting = true;
int invisibleCount = 0;
float winAngle;
int winner = -1;
bool winningShot = false;
int winningHyp = 0;
float winningZoom = 0.9985;
int winningRotations = 256;

CubiorObj cubior[cubiorCount];
CubeObj* cube[maxCubeCount];
GoalObj goal;
CameraObj camera[cubiorCount];
bool cameraDroppingIn[cubiorCount];
static int movementSpeed = 1;
static int jumpSpeedRatio = 5;
static int rotationSpeed = 10;
int maxJump = 25;
int maxSpeed = 20;
int maxCameraHeight = 2000;

int gravity = 2;

// Sfx Trigger Vars
bool justExited = false;
bool justPaused = false;
bool justUnpaused = false;
bool justChangedMenu = false;
bool justChangedOption = false;
bool justCausedError = false;
bool justTurnedCamera = false;
bool justFailedCamera = false;
bool justFocusedCamera = false;

// Pause option value
int option[4];
int maxOption[] = {1, // Title Screen
  4, // Main Menu
  4, // Pause
  4, // Options
  1, // Controls
  1, // Start Controls
  3 // Volume Settings
};
// And menu in which we are selecting said option
int menu[4];
/* Menus are as follows:
 * 0. Splash screen, option: Press Start OR Press Enter
 * 1. Start screen, options: Start/Options/Credits/Quit
 * 2. Pause menu, options: Resume/Drop Out/Options/Quit
 * 3. Options menu, options: Camera Controls, Controls, Back
 * 4. Controller map, option: Back
 * 5. Credits, option: Back
 */
// And if you've even started yet
bool started = false;

// Changing game state variables
bool gameplayRunning = true;
bool gameplayFirstRunning = true;
int xFar[4], xNear[4], zFar[4], zNear[4]; // for detecting walls for wall angles/shots
bool lastPlayerVisible[4]; // last time player visibility was reported
bool justHitStart = false; // Whether you just tried to start the game (forces you into controls view first)

// Keep track of player's visibility
int playerVisibleSlot = 0;
bool playerVisibleArray[4][playerVisibleMax];

// Networking
bool networkingEnabled = false;
bool onlineAlone = false;

// Quick math function for keepInBounds
int getEdge(int dimension, int neg) {
  return (neg*(dimension-(neg>0)*1)/2)*tileSize-neg*1;
}
// FIXME: This causes lots of lag right now. Intended to keep player inside game though
void keepInBounds(CubeObj* c1) {
    // The bounds are one fewer than the size of the grid
    // this makes checking collision in all directions from a cube never go out of bounds
    if (c1->getX()<=getEdge(currentMapWidth,-1)){c1->setX(getEdge(currentMapWidth,-1)+1);} // adding +1 here fixed edge problem
    if (c1->getX()>=getEdge(currentMapWidth,1)){c1->setX(getEdge(currentMapWidth,1));}
    if (c1->getY()<=getEdge(currentMapHeight,-1)){c1->setY(getEdge(currentMapHeight,-1)+1);}
    if (c1->getY()>=getEdge(currentMapHeight,1)){c1->setY(getEdge(currentMapHeight,1));}
    if (c1->getZ()<=getEdge(currentMapDepth,-1)){c1->setZ(getEdge(currentMapDepth,-1)+1);}
    if (c1->getZ()>=getEdge(currentMapDepth,1)){c1->setZ(getEdge(currentMapDepth,1));}
}

// Checks if any side of a cube is on the edge of the map
void findEdges(CubeObj* c1, CubeObj* map[][maxHeight][maxDepth]) {
  c1->setEdges(
    c1->getX() >=(currentMapWidth/2-1)*tileSize,
    c1->getX() <=-(currentMapWidth/2-1)*tileSize,
    c1->getY() >=(currentMapHeight/2-1)*tileSize,
    c1->getY() <=-(currentMapHeight/2-1)*tileSize,
    c1->getZ() >=(currentMapDepth/2-1)*tileSize,
    c1->getZ() <=-(currentMapDepth/2-1)*tileSize
  );
}

// Same as above, only with a map object
void findEdges(CubeObj* c1, Map* map) {
  c1->setEdges(
    c1->getX() >=(currentMapWidth/2-1)*tileSize,
    c1->getX() <=-(currentMapWidth/2-1)*tileSize,
    c1->getY() >=(currentMapHeight/2-1)*tileSize,
    c1->getY() <=-(currentMapHeight/2-1)*tileSize,
    c1->getZ() >=(currentMapDepth/2-1)*tileSize,
    c1->getZ() <=-(currentMapDepth/2-1)*tileSize
  );
}

void resetCubiors() {
    for (int i=0; i<cubiorCount; i++) {
        resetCubior(i);
    }
}

void resetLocalCubiors() {
    for (int i=0; i<cubiorCount; i++) {
        if (cubiorPlaying[i] && !cubiorOnline[i]) {
            resetCubior(i);
        }
    }
}

// Put a Cubior back in its start spot
void resetCubior(int i) {
  // And reset its visible-yet-intended count
  camera[i].setVisibleIntended(0);
  
  // Put cubior in falling spot
	int distFromCenter = (i+1)/2;
	int directionFromCenter = 1+(i%2)*(-2);
  //cubior[i].setPos(-200*(distFromCenter*directionFromCenter),100, currentMapWidth*tileSize*1/2-400);
  cubior[i].setPos(i*200-300,1000, currentMapDepth*tileSize/2-tileSize*(1+padding));

  cubior[i].resetToldToMove();
  cubior[i].setMomentumX(0);
  cubior[i].setMomentumY(0);
  cubior[i].setMomentumZ(0);
  cubior[i].moveX(0);
  cubior[i].moveY(3);
  cubior[i].moveZ(-3);

  // After Cubior is reset, THEN move camera
  // Put camera in drop down spot
  camera[i].resetPos();

}

void setupPlayers() {
    // Give initial positions/camera positions
    for (int i=0; i<cubiorCount; i++) {
        
        // Start camera!
        camera[i].alwaysFollow(&cubior[i],&goal,i);
        cameraDroppingIn[i] = true;
        
        // After set to always follow, THEN position camera and cubior
        // Starting camera and player pos
        resetCubior(i);
        keepInBounds(&cubior[i]);
	    camera[i].resetPos();
        cameraCube.setPos(camera[i].getX(),camera[i].getY(),camera[i].getZ());
        
        // Cubior Start State
        cubior[i].setPlayerNum(i);
        cubior[i].setHappiness(1.0-(i%4)*1.0/4); // was 1.0-i*1.0/cubiorCount before,
                                                 // now made to loop every 4 cubes
    }
    
    // Load nobody if only first running!
    //if (!gameplayFirstRunning) {
    bool anyonePlaying = false;
    for (int i=0; i<cubiorCount; i++) {
        anyonePlaying = anyonePlaying || cubiorPlaying[i];
    }
    // If NOBODY playing...
    if (started && !anyonePlaying) {
        // Use the chosen player
        int chosenOne = getPlayerChosen();
        if (chosenOne > -1 && chosenOne < cubiorCount) {
            cubiorPlaying[chosenOne] = true;
        } else {
            // Then ensure at least P1 is playing
            cubiorPlaying[0] = true;
        }
    }
    //}
}

void disablePlayers() {
    //playerPause(-1, true);
    for (int i=0; i<cubiorCount; i++) {
        cubiorPlaying[i] = 0;
        cubiorOnline[i] = 0;
    }
}

void setupNetworking(string addressToJoin) {
#ifdef enet_lib
    // Gameplay Start network stuff!
    // Only change networkingEnabled status if told to.
    // Empty string means no change
    if (addressToJoin.compare("") != 0) {
        networkingEnabled = (addressToJoin.compare("n") != 0);
    }
    if (networkingEnabled) {
        connectTo(addressToJoin);
        if (addressToJoin.compare("127.0.0.1") == 0) {
            onlineAlone = true;
        }
    }
#endif
}

void setupNetworkedPlayers() {
    // FIXME: have a real implementation here later. This is just for testing
    //bool networkedPlayer = true;
    //addPlayer(networkedPlayer);
    // changed my mind, going to do this later so it starts after first level starts...?
    
    // Need a new system for menu navigation so it's not based on players playing
}

void readLevel(string levelToLoad) {
    levelMap = MapReader::readMap(levelToLoad); // now load that level!
    currentMapWidth = levelMap->getWidth();
    currentMapHeight= levelMap->getHeight();
    currentMapDepth = levelMap->getDepth();
    cubeCount = levelMap->getCubeCount();
    if (currentMapWidth > playableWidth) { currentMapWidth = playableWidth; }
    if (currentMapHeight> playableHeight){ currentMapHeight= playableHeight;}
    if (currentMapDepth > playableDepth) { currentMapDepth = playableDepth; }
    if (cubeCount > maxCubeCount) { cubeCount = maxCubeCount; }
}

void setupLevel() {
    // Load cubes in from level reader
    int currentCube = 0;
    for (int z=0; z<levelMap->getDepth(); z++) {
        for (int x=0; x<levelMap->getWidth(); x++) {
            for (int y=0; y<levelMap->getHeight(); y++) {
                if (levelMap->getCubeAt(x,y,z) != 0 && currentCube < cubeCount) {
                    // Removing surrounded cubes increases lag, actually
                    //if (!levelMap->isSurrounded(x,y,z)) {
                    // FIXME: Should just grab cubeAt and put it in cube array. Cube array must be pointer then?
                    cube[currentCube] = levelMap->getCubeAt(x,y,z);
                    cube[currentCube]->setPos(tileSize*(x-levelMap->getWidth()/2),tileSize*(y-levelMap->getHeight()/2),tileSize*(z-levelMap->getDepth()/2));
                    //cube[currentCube]->setMaterial(levelMap->getCubeAt(x,y,z)->getMaterial());
                    //cube[currentCube]->setInvisible(levelMap->getCubeAt(x,y,z)->isInvisible());
                    currentCube++;
                    //}
                }
            }
        }
    }
    
    // Then the goal
    goal.setPos(levelMap->getGoalWidth(),levelMap->getGoalHeight(),levelMap->getGoalDepth());
    
    // Then populate permamap
    // ... with permanent Cubes
    for (int i = 0; i<cubeCount; i++) {
        cube[i]->tick();
        //keepInBounds(&cube[i]);
        addToCollisionMap(cube[i], permanentMap);
    }
    // Then set their neighbors, for more efficient rendering
    for (int i = 0; i<cubeCount; i++) {
        findNeighbors(cube[i], permanentMap);
        findEdges(cube[i], permanentMap);
    }
}

// Initialization for all gameplay, also triggering other initializations
void gameplayInit(string levelToLoad, string addressToJoin) {
  keyboardInit();
#ifdef enet_lib
  networkingInit();
#endif
  gameplayStart(levelToLoad, addressToJoin);
}

// Load in a level and set it up
void gameplayStart(string levelToLoad, string addressToJoin) {

  // If first running, disable all cubiors
  if (gameplayFirstRunning) {
    //permanentMap = NULL;
      disablePlayers();
      
      setupNetworking(addressToJoin);
      if (networkingEnabled) {
#ifdef enet_lib
          networkTick();
#endif
      }
  }
    
  if (gameplayRunning) {
    // First wipe the current map
    wipeCurrentMap(permanentMap);
    
    // This one does not work for some reason. permanentMap->wipeMap();
    // and thus the memory leak was vanquished, and it was good.
    delete levelMap;

    // And reset goal glow
    goal.setGlow(false);

    // Then read in a new map
    readLevel(levelToLoad);
      
    // Setup player positions and cameras
    setupPlayers();
    
    // And networked players
    if (networkingEnabled) {
        setupNetworkedPlayers();
    }
    
    // Setup level geometry and goal
    setupLevel();
  }
    
  // Temp fix for title screen, auto pause on start
  if (gameplayFirstRunning) {
    // initGameplay type of thing here for whole game, or start screen again.
    //camera[0].setAngleX(-90);
    // Reset everyone except p1's playability to false
    // So that we only see one view of the title screen
    for (int i=0; i<cubiorCount; i++) {
      cubiorPlaying[i] = false;
    }
    // Reset all to splash screen menu
    for (int i=0; i<cubiorCount; i++) {
      menu[i] = 0;
    }
    playerPause(-1,true, false);
  }
  
  // TEMPFIX: this fixes the title screen issue from the new addPlayer system
  menu[0] = 0;
    
}

// To count down to loading the next level
void nextLevelCountdown(int i) {

  winner = i;
  
  //cout << "Start countdown? Well..." << endl;
  // Same as the pivot point for upcoming rotation
  int targetX = cubior[winner].getX();
  int targetZ = cubior[winner].getZ();

  //cout << "targetX and Z are " << targetX << " and " << targetZ << endl;
  
  // Then capture this moment
  winAngle = getAngleBetween(camera[i].getX(),camera[i].getZ(),targetX,targetZ);
  //cout << "winAngle is " << winAngle << endl;
  
  // And tell the world someone just won
  winningShot = true;
  winningHyp = camera[winner].groundDistToPlayer();
  if (winningHyp > camera[winner].getFarthestDist()) {
    winningHyp = camera[winner].getFarthestDist();
  }
  //cout << "winningShot is " << winningShot << endl;
  
}

// Restart game by going back to first level
// aka resetGame
void restartGame(int i) {
  //setMenu(i, 0);
  //gameplayRunning = false;
  //gameplayFirstRunning = true;
  resetCubiors();
  loadLevel(0);

  for (int i=0; i<cubiorCount; i++) {
    setMenu(i,0); // start menu
  }
}

// To load the next level
void nextLevel() {
  loadLevel((currentLevel + 1) % totalLevels);
}

// So keyboard can know if pause press is also choosing a player
bool getGameplayFirstRunning() {
  return gameplayFirstRunning;
}
void setGameplayFirstRunning(bool b) {
  gameplayFirstRunning = b;
}

// To load the last level
void lastLevel() {
  loadLevel((currentLevel - 1 + totalLevels) % totalLevels);
}

// Moving to any level number
void loadLevel(int levelNum) {
  // Cannot load level if paused
  if (!gameplayRunning) {
    // So unpause it if needed!
    startGameplay();
    setJustUnpaused(true);
  }
  
  // Finally moving on, reset the winner
  winner = -1;
  winningShot = false;
  
  // Set next level number
  changeLevel = true;
  currentLevel = levelNum;
  gameplayFirstRunning = (levelNum == 0);
  
  // Then load the appropriate level
  int n;
  char buffer[100];
  n=sprintf(buffer, "./maps/cubiorMap%i.cubior", currentLevel); // should be relative/local but... for now, doing this so that trailer can get made in a timely matter
  gameplayStart(buffer, "");
  initVisuals();
}

// Just give number of current level
int getLevelNum() {
  return currentLevel;
}

// gameplayTick(), basically, or if it were an object, gameplay::tick()
// the main loop that gets called for every frame of gameplay
void gameplayLoop() {
  // cout << "Cubior playable " << 0 << " is " << cubiorPlaying[0] << " with online status " << cubiorOnline[0] << endl;
    
  /*cout << "New loop!" << endl;
  for (int i=0; i<4; i++) {
    cout << "Menu[" << i << "] = " << menu[i] << endl;
    cout << "Option[" << i << "] = " << option[i] << endl;
  }*/
  // NEWDELETEME: cout << "player to goal angle = " << getAngleBetween(cubior[0].getX(),cubior[0].getZ(),goal.getX(),goal.getZ()) << endl;
  //cout << "gameloop: camera[i] pos is " << camera[0].getX() << ", " << camera[0].getY() << ", " << camera[0].getZ() << endl;
  
  if (gameplayRunning && !winningShot) {
    //cout << cubior[0].getStrength() << " makes " << cubior[0].getMomentumX() << ", " << cubior[0].getMomentumZ() << " = " << sqrt(pow((float)cubior[0].getMomentumZ(),2) + pow((float)cubior[0].getMomentumX(), 2)) << endl;
	  
	  // Only recognize a level change for one loop
	  if (changeLevel) { changeLevel = false; }
	  
    wipeCurrentMap(collisionMap);

    // For players, start with one with lowest landedOnCount
    for(int landedOnNum = 0; landedOnNum < cubiorCount; landedOnNum++) {
      // Run main tick loop for all Cubiors...
      for (int i = 0; i<cubiorCount; i++) {
        if (cubiorPlaying[i] && cubior[i].getLandedOnCount() == landedOnNum) {
          //cout << "Ticking cubior " << i << endl;
          cubior[i].tick();
          keepInBounds(&cubior[i]);
          addToCollisionMap(&cubior[i], collisionMap);
          //cout << "Done ticking cubior " << i << " and " << i << "'s collision stuff" << endl;
        }
      }
    }
    // and the goal
    goal.tick();
    addToCollisionMap(&goal, collisionMap);

    // Then check collision against all other obstacles (cubes/cubiors)
	  for (int i = 0; i<cubiorCount; i++) {
          //cout << "This cubior " << i << " has online status " << cubior[i].getOnline() << endl;
      if (cubiorPlaying[i]) {

        // First, stop that dropping in if it's no longer happening
        if (cameraDroppingIn[i]) {
          if (!camera[i].getDroppingIn()) {
            cameraDroppingIn[i] = camera[i].getDroppingIn();
          }
        }

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
	    if (cubiorPlaying[i]){

        // Always check that camera isn't inside walls
        if (automatedCamera) {
            checkCameraAgainstWalls(i);
        }

        // Basic setup
        camera[i].tick();
        
        if (automatedCamera) {
            // So long as no intendedPos, locked to player, or commanded, try collision
            if (!camera[i].getFoundIntendedPos()) {
                checkCameraCubeCollision(i);
            }
            // If not in goal's range, ensure visibility
            if ((camera[i].goalOutsideDistRange() || !camera[i].goalWithinJumpRange()) ){
                ensurePlayerVisible(i);
            }
        }
            
        // If using a zooming camera, constantly and instantly zoom to visibility
        if (zoomingCamera) {
            //cout << "zooming right now is " << zoomingCameraRightNow << endl;
            if (!playerVisible(i)) {
                //cout << "NOT" << endl;
                moveToPlayer(i);
                zoomingCameraRightNow = true;
                camera[i].resetLOSDist();
            } else if (zoomingCameraRightNow) {
                //cout << "Found" << endl;
                // Set this as camera's new intended/commanded distance
                camera[i].resetLOSDist();
                zoomingCameraRightNow = false;
            } else {
                //cout << "ok" << endl;
            }
        }
      }
    }

  } else if (winningShot) {

    // How to handle the victory screen
    rotateAroundPlayer(winner, winningHyp);
    winningHyp *= winningZoom;
    
    // Same as the pivot point for rotation
    int targetX = camera[winner].getPermanentTarget()->getX();
    int targetZ = camera[winner].getPermanentTarget()->getZ();
    float delta = abs(winAngle-getAngleBetween(camera[winner].getX(),camera[winner].getZ(),targetX,targetZ));
    while (delta >= 2*M_PI) { delta -= 2*M_PI; }
    while (delta <=-2*M_PI) { delta += 2*M_PI; }
    
    // Used so we don't immediately jump to next level
    // (must have same angle as start AND been glowing for 10 steps)
    goal.incrementGlowCount();
    // If made a full rotation, and not the first step of glowing, then onwards!
    if (delta < (M_PI/winningRotations) && goal.getGlowMax()) {
      nextLevel();
      goal.resetGlowCount();
    }
  }

  // Lastly, network stuff!
    
  // Networking anytime loop
#ifdef enet_lib
  if (networkingEnabled) {
    networkTick();
    for (int i=0; i<cubiorCount; i++) {
      //cout << "player " << i << " is " << getOnline(i) << ", ";
      if (cubiorOnline[i] != getOnline(i)) {
        cubiorOnline[i] = getOnline(i);
        // Newly added? Add! Newly deleted? Delete!
        if (cubiorOnline[i]) {
          cout << "Added online " << i << endl;
          addPlayer(i, true);
        } else {
          cout << "Removed online " << i << endl;
          removePlayer(i);
        }
      }
    }
  }
  
  // Networking gameplay loop
  if (networkingEnabled && gameplayRunning && !gameplayFirstRunning) {
    
    // Prep your own position to send out
    // (only sending first player's position atm)
    for (int i = 0; i<cubiorCount; i++) {
      // Check if local before sending out
      // (FIXME: right now, only the last in order gets counted)
      // (by overriding all others, being the last to be true)
      if (!cubiorOnline[i]) {
        // Set playing state first
        setOnline(i, cubiorPlaying[i]); // not efficient since setting when unnecessary?
        // then if playing, set all other states
        if (cubiorPlaying[i]) {
          setPosX(i, cubior[i].getX());
          setPosY(i, cubior[i].getY());
          setPosZ(i, cubior[i].getZ());
          setMomentum(i, cubior[i].getMomentum());
          setDirection(i, cubior[i].getToldDirection());
        }
      }
    }

    // Networking was here!
    // it used to be, that is. Between setting and before setting onlines' pos
    
    int modifier = onlineAlone ? 200 : 0;

    // Find other players that are online, and represent them
    for (int i=0; i<cubiorCount; i++) {
        if (cubiorOnline[i]) {
            cubior[i].setPos(getPosX(i)+modifier, getPosY(i), getPosZ(i)+modifier);
            cubior[i].setMomentum(getMomentum(i));
            cubior[i].setToldDirection(getDirection(i));
        }
    }
    
  }
#endif
}

// Use the camera cube to check for collision against the map
void checkCameraCubeCollision(int i) {
    // And bounce off walls if colliding
    cameraCube.setPos(camera[i].getX(),camera[i].getY(),camera[i].getZ());
    camera[i].setCameraStatus(true);
    // using cameraCube here since a lack thereof make camera's collision stop working
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
    //camera[i].setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
}

// See if you can line the camera up against a wall
// so the player has an easier time navigating the platforming
// (out here in gameplay to more easily use the cube array)
void checkCameraAgainstWalls(int i) {
  //cout << "even checking? " << endl;
  // try to line camera up against a wall
  int xWall[4];
  int zWall[4];
  searchForWall(i,xWall,permanentMap,0);
  searchForWall(i,zWall,permanentMap,2);
  //cout << "xWall is [" << xWall[0] << "," << xWall[1] << "," << xWall[2] << "," << xWall[3] << "]" << endl;
  //cout << "zWall is [" << zWall[0] << "," << zWall[1] << "," << zWall[2] << "," << zWall[3] << "]" << endl;
  // FIXME: I want this to be based on an "is any space or wall" bool
  /*if (xWall != 0 || zWall != 0) {
    cout << "atan is " << atan(zWall*1.0/xWall) << endl;
    rotateToAngle(i,atan(zWall*1.0/xWall),camera[i].groundDistToPlayer());
  }*/
        
  //cout << "for2loop: camera[i] pos is " << camera[i].getX() << ", " << camera[i].getY() << ", " << camera[i].getZ() << endl;
  //cout << "grounded = " << cubior[i].getGrounded() << endl;
  //cout << "stillGrounded = " << cubior[i].getStillGrounded() << endl;
        
  // Only look for new walls if not moving much, not dropping in, and no recent player camera commands
  if (!cubior[i].isMovingQuickly() && !cameraDroppingIn[i]) {
    if (!automatedCamera || playerVisible(i)) { // not sure what's automated here
      if (cubior[i].getStillGrounded()) {
    // Keep track of how often each direction is requested.
    xFar[i] = 0, xNear[i] = 0, zFar[i] = 0, zNear[i] = 0;

    // Look for walls
    if (xWall[0] || xWall[1] || zWall[0] || zWall[1]) {
      if (xWall[0] && !xWall[1]) { xNear[i]++; } // near wall but no far wall
      if (!xWall[0] && xWall[1]) { xFar[i]++;  } // far wall but no near wall
      if (zWall[0] && !zWall[1]) { zNear[i]++; } // near wall but no far wall
      if (!zWall[0] && zWall[1]) { zFar[i]++;  } // far wall but no near wall
    }
    // No walls? Look for drops!
    if (xWall[2] || xWall[3] || zWall[2] || zWall[3]) {
      if (xWall[2]) { xNear[i]++; } // far space
      if (xWall[3]) { xFar[i]++;  } // near space
      if (zWall[2]) { zNear[i]++; } // far space
      if (zWall[3]) { zFar[i]++;  } // near space
    }
      }
    }
  }

  // Do not try to adjust for walls if in goal range
  if ((!automatedCamera || playerVisible(i)) && // automates a stopper on command rotating camera into a wall
    // must also either already be locked
    (camera[i].getLockedToPlayerX() || camera[i].getLockedToPlayer() || camera[i].getLockedToPlayerZ() /*||
    // or not be in goal range
    !camera[i].goalWithinJumpRange() || (camera[i].goalOutsideDistRange())*/) &&
    // Then also, can't be told to not move, and must have a wall or space to align to
    !camera[i].getJustFixedVisibility() && (xNear[i] || xFar[i] || zNear[i] || zFar[i])) {

    camera[i].tryingXNear(false);
    camera[i].tryingXFar(false);
    camera[i].tryingZNear(false);
    camera[i].tryingZFar(false);
    float targetAngle = 0;

    // xNear wins
    if (xNear[i] >= xFar[i] && xNear[i] >= zFar[i] && xNear[i] >= zNear[i]) {
      camera[i].tryingXNear(true);
      targetAngle = 0;
      if (abs(targetAngle - camera[i].getRadiansAngleY())>0.04) {
        rotateToAngle(i,targetAngle,camera[i].groundDistToPlayer());
      } else if (!camera[i].getLockedToPlayerX()) {
        camera[i].setLockedToPlayerX(true);
      }
    // xFar wins
    } else if (xFar[i] >= xNear[i] && xFar[i] >= zFar[i] && xFar[i] >= zNear[i]) {
      camera[i].tryingXFar(true);
      targetAngle = M_PI;
      if (abs(targetAngle - camera[i].getRadiansAngleY())>0.04) {
        rotateToAngle(i,M_PI,camera[i].groundDistToPlayer());
      } else if (!camera[i].getLockedToPlayerX()) {
        camera[i].setLockedToPlayerX(true);
      }
    } else if (camera[i].getLockedToPlayerX()) { camera[i].setLockedToPlayerX(false); }
    // zNear wins
    if (zNear[i] >= xFar[i] && zNear[i] >= zFar[i] && zNear[i] >= xNear[i]) {
      camera[i].tryingZNear(true);
      targetAngle = 1.0/2*M_PI;
      if (abs(targetAngle - camera[i].getRadiansAngleY())>0.04) {
        rotateToAngle(i,targetAngle,camera[i].groundDistToPlayer());
      } else if (!camera[i].getLockedToPlayerZ()) {
        camera[i].setLockedToPlayerZ(true); 
      }
    // zFar wins
    } else if (zFar[i] >= xFar[i] && zFar[i] >= xNear[i] && zFar[i] >= zNear[i]) {
      camera[i].tryingZFar(true);
      // to figure out which direction to rotate towards
      int targetX = camera[i].getPermanentTarget()->getX();
      int targetZ = camera[i].getPermanentTarget()->getZ();
      float testAngle = getAngleBetween(camera[i].getX(),camera[i].getZ(),targetX,targetZ);
      // Then rotate in that direction
      if (testAngle < M_PI/2.0) {
        targetAngle = -0.999/2*M_PI;
      } else {
        targetAngle = 2.999/2*M_PI;
      }
      // Put them in the same range
      while (targetAngle < camera[i].getRadiansAngleY() - M_PI) { targetAngle += 2*M_PI; }
      while (targetAngle > camera[i].getRadiansAngleY() + M_PI) { targetAngle -= 2*M_PI; }
      if (abs(targetAngle - camera[i].getRadiansAngleY())>0.07) {
        rotateToAngle(i,targetAngle,camera[i].groundDistToPlayer());
      } else if (!camera[i].getLockedToPlayerZ()) {
        camera[i].setLockedToPlayerZ(true);
      }
    } else if (camera[i].getLockedToPlayerZ()) { camera[i].setLockedToPlayerZ(false); }
  } else {
    if (camera[i].getLockedToPlayer())  { camera[i].setLockedToPlayer(false); }
    if (camera[i].getLockedToPlayerX()) { camera[i].setLockedToPlayerX(false); }
    if (camera[i].getLockedToPlayerZ()) { camera[i].setLockedToPlayerZ(false); }
  }
}

/*******************
 * Check for Walls *
 *******************/

// Looks for vertical walls or clearings along 1 dimension of player
int* searchForWall(int player, int results[], CubeObj* m[][maxHeight][maxDepth], int dimension) {
  bool rearWall = 0, frontWall = 0, rearSpace = 0, frontSpace = 0;
  int cX = getCollisionMapSlot(&cubior[player],0);
  int cY = getCollisionMapSlot(&cubior[player],1);
  int cZ = getCollisionMapSlot(&cubior[player],2);
  //cout << "Currently viewing from " << cX << ", " << cY << ", " << cZ << endl;
  // delta along cube radius
  int dX = 0, dZ = 0;
  
  // Now check surroundings on X or Z axis
  for (int i=0; i<wallCheckRadius; i++) {
    // depending on dimension, choose which axis to alter
    if (dimension == 0) { dX = i;}
    if (dimension == 2) { dZ = i;}
    // check in front
    if (!frontWall && insideMap(cX+dX,cY,cZ+dZ)) { 
      frontWall = ((m[cX+dX][cY][cZ+dZ] != NULL) && !(m[cX+dX][cY][cZ+dZ]->isInvisible()) && (m[cX+dX][cY][cZ+dZ]->isVertWall())); 
    }
    //cout << "frontWall on " << dimension << " at " << cX+dX << ", " << cY << ", " << cZ+dZ << " is " << frontWall << " with existence as " << (m[cX+dX][cY][cZ+dZ] != NULL) << endl;
      // or for a lack behind
    if (!frontSpace && insideMap(cX-dX,cY,cZ-dZ)) { 
      frontSpace= ((m[cX-dX][cY][cZ-dZ] == NULL || m[cX-dX][cY][cZ-dZ]->isInvisible()) && (isVertSpace(m,cX-dX,cY,cZ-dZ)));
    }
      // check behind
    if (!rearWall && insideMap(cX-dX,cY,cZ-dZ)) { 
      rearWall  = ((m[cX-dX][cY][cZ-dZ] != NULL) && !(m[cX-dX][cY][cZ-dZ]->isInvisible()) && (m[cX-dX][cY][cZ-dZ]->isVertWall())); 
    }
    //cout << "rearWall  on " << dimension << " at " << cX-dX << ", " << cY << ", " << cZ-dZ << " is " << rearWall << " with existence as " << (m[cX-dX][cY][cZ-dZ] != NULL) << endl;
    // or for a lack in front
    if (!rearSpace && insideMap(cX+dX,cY,cZ+dZ)) { 
      rearSpace = ((m[cX+dX][cY][cZ+dZ] == NULL || m[cX+dX][cY][cZ+dZ]->isInvisible()) && (isVertSpace(m,cX+dX,cY,cZ+dZ)));
    }
  }
  results[0] = frontWall;
  results[1] = rearWall;
  results[2] = frontSpace;
  results[3] = rearSpace;
  return results;
}

// Same as above, only for maps instead of arrays
int* searchForWall(int player, int results[], Map* m, int dimension) {
  bool rearWall = 0, frontWall = 0, rearSpace = 0, frontSpace = 0;
  int cX = getCollisionMapSlot(&cubior[player],0);
  int cY = getCollisionMapSlot(&cubior[player],1);
  int cZ = getCollisionMapSlot(&cubior[player],2);
  // delta along cube radius
  int dX = 0, dZ = 0;
  
  // Now check surroundings on X or Z axis
  for (int i=0; i<wallCheckRadius; i++) {
    // depending on dimension, choose which axis to alter
    if (dimension == 0) { dX = i;}
    if (dimension == 2) { dZ = i;}
    // check in front
    if (!frontWall && insideMap(cX+dX,cY,cZ+dZ)) { 
      frontWall = ((m->getCubeAt(cX+dX,cY,cZ+dZ) != NULL) && !(m->getCubeAt(cX+dX,cY,cZ+dZ)->isInvisible()) && (m->getCubeAt(cX+dX,cY,cZ+dZ)->isVertWall())); 
    }
    // or for a lack behind
    if (!frontSpace && insideMap(cX-dX,cY,cZ-dZ)) { 
      frontSpace= ((m->getCubeAt(cX-dX,cY,cZ-dZ) == NULL || m->getCubeAt(cX-dX,cY,cZ-dZ)->isInvisible()) && (isVertSpace(m,cX-dX,cY,cZ-dZ)));
    }
    // check behind
    if (!rearWall && insideMap(cX-dX,cY,cZ-dZ)) { 
      rearWall  = ((m->getCubeAt(cX-dX,cY,cZ-dZ) != NULL) && !(m->getCubeAt(cX-dX,cY,cZ-dZ)->isInvisible()) && (m->getCubeAt(cX-dX,cY,cZ-dZ)->isVertWall())); 
    }
    // or for a lack in front
    if (!rearSpace && insideMap(cX+dX,cY,cZ+dZ)) { 
      rearSpace = ((m->getCubeAt(cX+dX,cY,cZ+dZ) == NULL || m->getCubeAt(cX+dX,cY,cZ+dZ)->isInvisible()) && (isVertSpace(m,cX+dX,cY,cZ+dZ)));
    }
  }
  results[0] = frontWall;
  results[1] = rearWall;
  results[2] = frontSpace;
  results[3] = rearSpace;
  return results;
}

// Check if equiv of isVertWall, but all spaces empty
bool isVertSpace(CubeObj* m[][maxHeight][maxDepth], int cX, int cY, int cZ) {
  CubeObj* self  = m[cX][cY][cZ];
  CubeObj* left  = m[cX+1][cY][cZ];
  CubeObj* right = m[cX-1][cY][cZ];
  CubeObj* up    = m[cX][cY+1][cZ];
  CubeObj* down  = m[cX][cY-1][cZ];
  CubeObj* front = m[cX][cY][cZ+1];
  CubeObj* back  = m[cX][cY][cZ-1];
  // if not at an edge
  if ((cX<maxWidth && cX>0) && (cY<maxHeight && cY>0) && (cZ<maxDepth && cZ>0)){
    return ((self == NULL || self->isInvisible()) // center space
      && ((up   == NULL ||   up->isInvisible()) &&
          (down == NULL || down->isInvisible())) // vert neighbors
      && (  // some combo of horiz neighbors
         ((left  == NULL ||  left->isInvisible()) &&
          (right == NULL || right->isInvisible())) // x neighbors
      || ((front == NULL || front->isInvisible()) &&
          (back  == NULL ||  back->isInvisible())) // z neighbors
      ));
  } else {
    // edge cases always count as clearings
    // FIXME: haven't really thought this through too well, to be honest
    return false;
  }
}

// Check if equiv of isVertWall, but all spaces empty
bool isVertSpace(Map* m, int cX, int cY, int cZ) {
  CubeObj* self  = m->getCubeAt(cX,cY,cZ);
  CubeObj* left  = m->getCubeAt(cX+1,cY,cZ);
  CubeObj* right = m->getCubeAt(cX-1,cY,cZ);
  CubeObj* up    = m->getCubeAt(cX,cY+1,cZ);
  CubeObj* down  = m->getCubeAt(cX,cY-1,cZ);
  CubeObj* front = m->getCubeAt(cX,cY,cZ+1);
  CubeObj* back  = m->getCubeAt(cX,cY,cZ-1);
  // if not at an edge
  if ((cX<maxWidth && cX>0) && (cY<maxHeight && cY>0) && (cZ<maxDepth && cZ>0)){
    return ((self == NULL || self->isInvisible()) // center space
      && ((up   == NULL ||   up->isInvisible()) &&
          (down == NULL || down->isInvisible())) // vert neighbors
      && (  // some combo of horiz neighbors
         ((left  == NULL ||  left->isInvisible()) &&
          (right == NULL || right->isInvisible())) // x neighbors
      || ((front == NULL || front->isInvisible()) &&
          (back  == NULL ||  back->isInvisible())) // z neighbors
      ));
  } else {
    // edge cases always count as clearings
    // FIXME: haven't really thought this through too well, to be honest
    return false;
  }
}

// Move camera to new angle, for wall angles
void rotateToAngle(int i, float targetAngle, int hyp) {
  
  // Pivot point for rotation
  int targetX = camera[i].getPermanentTarget()->getX();
  int targetZ = camera[i].getPermanentTarget()->getZ();
  // Position we will move to on each turn
  int oldX = camera[i].getX();
  int oldY = camera[i].getY();
  int oldZ = camera[i].getZ();
  int newX = oldX;
  int newZ = oldZ;
  	
  // Angle that we will be moving away from, pivot point side
  //cout << "targetAngle is " << targetAngle << endl;
  float baseAngle = getAngleBetween(camera[i].getX(),camera[i].getZ(),targetX,targetZ);
  //cout << "baseAngle is " << baseAngle << endl;
  //while (targetAngle < -M_PI/2) { targetAngle += M_PI*2; }
  //while (targetAngle > M_PI*3.0/2) { targetAngle -= M_PI*2; }
  //baseAngle = camera[i].matchRangeOf(baseAngle, targetAngle);
  // Angle we will be moving to, based on pivot
  float newAngle = baseAngle;
  //cout << "baseAngle is " << baseAngle << endl;
  // rotate until player is visible or 180 from start
  // New pivot angle to go to
  while (targetAngle > baseAngle + M_PI) { baseAngle += 2*M_PI; }
  while (targetAngle < baseAngle - M_PI) { baseAngle -= 2*M_PI; }
  //cout << "ROTATING FROM " << baseAngle << " TO " << targetAngle << endl;
  if (abs(targetAngle-baseAngle)>(M_PI*2.0/(winningRotations))) {
    newAngle = baseAngle + (1-2*(baseAngle>targetAngle))*(M_PI*2.0/(winningRotations));
  } else {
    // too close, just make equal!
    newAngle = targetAngle;
  }
  //cout << "ACTUALLY ROTATING TO " << newAngle << endl;
  //cout << "newAngle is " << newAngle << endl;
  // Set new intended pos for each turn
  // math is a little hackey, tried swapping cos and sin and adding M_PI
  // and the numbers looked better, and camera worked better
  newX = targetX + hyp*cos(M_PI+newAngle);
  newZ = targetZ + hyp*sin(M_PI+newAngle);

  // Leaving camera cube in here just in case forgetting it would ruin something, not sure
  // FIXME later by testing it with no camera cube step
  // Looks like it is necessary. At the least, it was necessary for drop in of
  // next level after rotating around goal, and it still makes or breaks locking
  // to x and z axis against walls
  // Oh, didn't notice camera's setPos relying on it though. Might want to fix that and try again.
  cameraCube.setPos(newX,camera[i].getY(),newZ);
  camera[i].setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
  camera[i].lookAtTarget();
  camera[i].updateCamArray();
  camera[i].updateMeans();
  //cout << "finalAngle is " << getAngleBetween(camera[i].getX(),camera[i].getZ(),targetX,targetZ) << endl;
}
    
/*************
 * Check LOS *
 *************/

// This checks if the player is visible, and fixes invisible cases too
void ensurePlayerVisible(int i) {
  //FIXME: Only need this cout for understanding range of angles or how they work
  //cout << "baseAngle: " <<  getAngleBetween(camera[i].getX(),camera[i].getZ(), camera[i].getPermanentTarget()->getX(), camera[i].getPermanentTarget()->getZ()) << endl;
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
      // If player is not moving or been invisible too long, feel free to move camera
      if (!cubior[i].isMoving() || invisibleCount >= invisibleMax) {
        fixPlayerVisibility(i);
        invisibleCount = 0;
      } else {
        invisibleCount++;
      }
    }
    //cout << "Camera " << i << "'s vis = " << camera[i].getLOS() << " & p1 to goal = " << camera[i].goalWithinNearRange() << endl;
  }
}

// Copy of playerVisible but for an array
bool playerRegularlyVisible(int i) {
  checkCameraLOS(&camera[i],permanentMap);
  playerVisibleArray[i][playerVisibleSlot] = camera[i].getLOS();
  int sum = 0;
  for (int k=0; k<playerVisibleMax; k++) {
    // add one for every slot where visible
    if (playerVisibleArray[i][k]) {
      sum++;
    }
  }
  playerVisibleSlot++;
  if (playerVisibleSlot >= playerVisibleMax) { playerVisibleSlot = 0; }
  // Must be visible half the time or more
  return (sum >= playerVisibleMax/2); 
}
// Gives player non-visibility
bool playerVisible(int i) {
  // Must check LOS first, or getLOS will not be updated
  checkCameraLOS(&camera[i],permanentMap);
  // then return the newly updated results
  bool result = camera[i].getLOS();
  lastPlayerVisible[i] = result;
  return  result;
}

// Gives last result for playerVisible
bool getLastPlayerVisible(int i) {
  return lastPlayerVisible[i];
}

// Gives player non-visibility
bool cubeVisible(int i, int j) {
  // Must check LOS first, or getLOS will not be updated
  return getCameraToCubeLOS(&camera[i],cube[j],permanentMap);
}

// We know the player is not visible, so fix it!
void fixPlayerVisibility(int i) {
  if (rotateIfInvisible) {
    if (!camera[i].getFoundIntendedPos()) {
      rotateToPlayer(i,0);
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
void rotateToPlayer(int i, int distDiff) { // distDiff is how much closer to be than cam angle
  //cout << "Rotating to player w/ distDiff " << distDiff << endl;
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
  int hyp = camera[i].groundDistToPlayer() - distDiff;
  if (hyp > camera[i].getFarthestDist()) {
    hyp = camera[i].getFarthestDist();
  }
  //cout << "GroundDistToPlayer: " << hyp << endl;
  //cout << "our own math for it " << (sqrt(pow(oldX-targetX,2)+pow(oldZ-targetZ,2))) << endl;
  
  // Angle that we will be moving away from, pivot point side
  float baseAngle = /*M_PI*3/2.0 - */ getAngleBetween(camera[i].getX(),camera[i].getZ(),targetX,targetZ);
  // Angle we will be moving to, based on pivot
  float newAngle = baseAngle;
  //cout << "oldX: " << oldX << ", oldX: " << oldZ << endl;
  //cout << "hyp: " << hyp << endl;
  //cout << "targetX: " << targetX << ", targetZ: " << targetZ << endl;
  //cout << "baseAngle, " << baseAngle << ", newAngle, " << newAngle << endl;
  
  //cout << "START" << endl;
  // rotate until player is visible or 180 from start
  for (int k = 0; k<anglesToTry; k++)
  {
    // New pivot angle to go to
    newAngle = baseAngle + (M_PI*2.0/anglesToTry)*k/2.0*(1.0-2.0*(k%2));
    
    //cout << "newAngle, " << newAngle << endl;
    // Set new intended pos for each turn
    // math is a little hackey, tried swapping cos and sin and adding M_PI
    // and the numbers looked better, and camera worked better
    newX = targetX + hyp*cos(M_PI+newAngle);
    newZ = targetZ + hyp*sin(M_PI+newAngle);
    //cout << "newX: " << newX << ", newZ: " << newZ << endl;
    
    //cout << "Target " << targetX << ", " << targetZ << endl;
    //cout << "hyp is " << hyp << " w/ newX " << newX << " and newZ " << newZ << endl;
    //cout << "newAngle is " << newAngle << " from baseAngle " << baseAngle << endl;
    
    // Leaving camera cube in here just in case forgetting it would ruin something, not sure
    // FIXME later by testing it with no camera cube step
    cameraCube.setPos(newX,camera[i].getY(),newZ);
    camera[i].setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
    
    // Once you see the player visible from an empty spot, remember it!
    // If a clear shot to the camera, use the position for sure.
    // May need to fix this later, as requires setting cam pos every time
    int cX = getCollisionMapSlot(&cubior[i],0);
    int cY = getCollisionMapSlot(&cubior[i],1);
    int cZ = getCollisionMapSlot(&cubior[i],2);  
    #ifdef newPermanentMap
      if (!automatedCamera || (playerVisible(i) && (permanentMap->getCubeAt(cX,cY,cZ) == NULL || permanentMap->getCubeAt(cX,cY,cZ)->isInvisible()))) {
    #else
        if (!automatedCamera || (playerVisible(i) && (permanentMap[cX][cY][cZ] == NULL || permanentMap[cX][cY][cZ]->isInvisible()))) {
    #endif
      // FIXME: I'm guessing this seemingly unneccessary tick is causing issues
      // since the camera kind of jumps when it starts to readjust every angle
      //camera[i].tick();
      foundAnAngle = true;
      //cout << "Found a fix! (might not be perfect)" << endl;
           
      // Next, test if it is also a straight shot from the current camera
      if (!automatedCamera || checkPathVisibility(&cameraCube,&oldCamera, permanentMap)) {

        //cout << "Actually, it is perfect!" << endl;
        // Found a straight shot? Remember it and stop looking for anything better!
        intendedPos.setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
        foundIntendedPos = true;
        
        // Then see if you can go one step further and have equal visibility
        // (basically, all of the above code squished together for round 2)
        int l = k + 2;
        float doubleNewAngle = baseAngle + (M_PI*2.0/anglesToTry)*l/2.0*(1.0-2.0*(l%2));
        int doubleNewX = targetX + hyp*cos(M_PI+doubleNewAngle);
        int doubleNewZ = targetZ + hyp*sin(M_PI+doubleNewAngle);
        cameraCube.setPos(doubleNewX,camera[i].getY(),doubleNewZ);
        camera[i].setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
        if (!automatedCamera || (playerVisible(i) && checkPathVisibility(&cameraCube,&oldCamera, permanentMap))) {
          intendedPos.setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());          
        }
        
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
  /*} else {
    int newDistDiff = 500; // how much closer to get each new try
    // Still a bit away from player?
    if (hyp > newDistDiff*2) {
      // Move closer and try again
      rotateToPlayer(i,distDiff+newDistDiff);
    }*/
  }
  //cout << "foundIntendedPos = " << (foundIntendedPos ? "true" : "false") << endl;
}

// Try to find an angle & rotate the camera to angle to see the player
void rotateAroundPlayer(int i, int hyp) {
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
  
  //cout << "GroundDistToPlayer: " << hyp << endl;
  //cout << "our own math for it " << (sqrt(pow(oldX-targetX,2)+pow(oldZ-targetZ,2))) << endl;
  
  // Angle that we will be moving away from, pivot point side
  float baseAngle = /*M_PI*3/2.0 - */ getAngleBetween(camera[i].getX(),camera[i].getZ(),targetX,targetZ);
  // Angle we will be moving to, based on pivot
  float newAngle = baseAngle;
  
  //cout << "START" << endl;
  // rotate until player is visible or 180 from start
  // New pivot angle to go to
  newAngle = baseAngle + (M_PI*2.0/(winningRotations));

  // Set new intended pos for each turn
  // math is a little hackey, tried swapping cos and sin and adding M_PI
  // and the numbers looked better, and camera worked better
  newX = targetX + hyp*cos(M_PI+newAngle);
  newZ = targetZ + hyp*sin(M_PI+newAngle);

  // Leaving camera cube in here just in case forgetting it would ruin something, not sure
  // FIXME later by testing it with no camera cube step
  cameraCube.setPos(newX,camera[i].getY(),newZ);
  camera[i].setPos(cameraCube.getX(),cameraCube.getY(),cameraCube.getZ());
  camera[i].lookAtTarget();
  camera[i].updateCamArray();
  camera[i].updateMeans();
}

// See if a path is clear between two positions in the 3d grid
bool checkSlotPathVisibility(int aX, int aY, int aZ, int gX, int gY, int gZ, CubeObj* m[][maxHeight][maxDepth]) {
  
  bool showData = false;
  int cX = aX, cY = aY, cZ = aZ;

  // Tracker moves along the line, getting as close as possible
  CubeObj tracker;
  tracker.setPos(slotToPosition(cX,0), slotToPosition(cY,1), slotToPosition(cZ,2));
  
  if (showData) {
    cout << "we were given " << aX << ", " << aY << ", " << aZ << endl;
    cout << "STAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAART!" << endl;
    cout << "goal @ <" << (gX) << ", " << (gY) << ", " << (gZ) << ">" << endl;
    cout << "were @ <" << tracker.getX() << ", " << tracker.getY() << ", " << tracker.getZ() << ">" << endl;
    cout << "Trying to reach [" << gX << ", " << gY << ", " << gZ << "]" << endl;
    cout << "Currently viewing [" << cX << ", " << cY << ", " << cZ << "]" << endl;
  }
  
  // If too far to tell, just pretend we're visible
  // don't make the camera spin before we're even in range.
  //if (cX < 0 || cX > maxWidth || cY < 0 || cY > maxHeight || cZ < 0 || cZ > maxDepth) {

  // if still dropping in or too far, just say "Yes, they're visible"
  if (abs(cX) > maxHeight || abs(cY) > maxHeight || abs(cZ) > maxHeight) {
    return true;
  }

  // Only tolerate not moving up to six times before quitting
  int sameSpot = 0;
  const int sameMax = 5;
  int sameX[sameMax];
  int sameY[sameMax];
  int sameZ[sameMax];
  sameX[0] = cX;
  sameY[0] = cY;
  sameZ[0] = cZ;
  sameSpot++;

  // While not arrived, search
  while(cX != gX || cY != gY || cZ != gZ) {

    CubeObj* focusedCube = m[cX][cY][cZ];
    // Found something there? Make sure it has 2D of neighbors (is a real threat)
    if ((cX < maxWidth && cX >= 0 && cY < maxHeight && cY >= 0 && cZ < maxDepth && cZ >= 0) && 
      focusedCube != NULL && !(focusedCube->isInvisible()) && focusedCube->isWall()) {
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
    // Otherwise, if *something* is a bigger dist than one away, keep moving closer!
    if (abs(cX-gX)>1 || abs(cY-gY)>1 || abs(cZ-gZ)>1) {
      tracker.changePosTowards(slotToPosition(gX,0),slotToPosition(gY,1),slotToPosition(gZ,2),tileSize/16.0); // was /4
    } else {
      // But if not, well, close enough! Next move would have been there anyways
        tracker.changePosTowards(slotToPosition(gX,0),slotToPosition(gY,1),slotToPosition(gZ,2),tileSize/32.0); // was /4
    }

    // Then Reset Cam Tracker Pos
    cX = getCollisionMapSlot(&tracker,0);
    cY = getCollisionMapSlot(&tracker,1);
    cZ = getCollisionMapSlot(&tracker,2);
    /*
    // Update the Same Spots
    sameX[sameSpot] = cX;
    sameY[sameSpot] = cY;
    sameZ[sameSpot] = cZ;
    sameSpot++;
    if (sameSpot >= sameMax) { sameSpot = 0; }
    // All the same? Quit!
    bool allSame = true;
    for (int s=0; s<sameMax-2; s++) {
      allSame = allSame && (sameX[s] == sameX[s+1] || sameX[s] == sameX[s+2]);
      allSame = allSame && (sameY[s] == sameY[s+1] || sameY[s] == sameY[s+2]);
      allSame = allSame && (sameY[s] == sameY[s+1] || sameY[s] == sameY[s+2]);
    }
    //cout << " then all the same is " << allSame;
    // No differences? Quit! Just pretend we can't see the player
    // Since we got stuck by even trying.
    if (allSame) {
      return false;
    }*/
    /*
    if (showData) {
      cout << "<" << tracker.getX() << ", " << tracker.getY() << ", " << tracker.getZ() << ">, ";
      cout << "[" << cX << ", " << cY << ", " << cZ << "] -> ";
      cout << "[" << gX << ", " << gY << ", " << gZ << "]" << endl;
    }*/
    
    //cout << "cX is " << cX << ", cY is " << cY << ", cZ is " << cZ << endl;
    //cout << "gX is " << gX << ", gY is " << gY << ", gZ is " << gZ << endl;
    //cout << "From (" << tracker.getX() << ", " << tracker.getY() << ", " << tracker.getZ() << ") to (" << c->getPermanentTarget()->getX() << ", " << c->getPermanentTarget()->getY() << ", " << c->getPermanentTarget()->getZ() << ")" << endl;
  }
  return true;
}

// Same as above, only for maps instead of arrays
bool checkSlotPathVisibility(int aX, int aY, int aZ, int gX, int gY, int gZ, Map* m) {
  bool showData = false;
  int cX = aX, cY = aY, cZ = aZ;

  // Tracker moves along the line, getting as close as possible
  CubeObj tracker;
  tracker.setPos(slotToPosition(cX,0), slotToPosition(cY,1), slotToPosition(cZ,2));
  
  // if still dropping in or too far, just say "Yes, they're visible"
  if (abs(cX) > maxHeight || abs(cY) > maxHeight || abs(cZ) > maxHeight) {
    return true;
  }

  // Only tolerate not moving up to six times before quitting
  int sameSpot = 0;
  const int sameMax = 5;
  int sameX[sameMax];
  int sameY[sameMax];
  int sameZ[sameMax];
  sameX[0] = cX;
  sameY[0] = cY;
  sameZ[0] = cZ;
  sameSpot++;

  // While not arrived, search
  while(cX != gX || cY != gY || cZ != gZ) {

    CubeObj* focusedCube = m->getCubeAt(cX,cY,cZ);
    // Found something there? Make sure it has 2D of neighbors (is a real threat)
    if ((cX < maxWidth && cX >= 0 && cY < maxHeight && cY >= 0 && cZ < maxDepth && cZ >= 0) && 
      focusedCube != NULL && !(focusedCube->isInvisible()) && focusedCube->isWall()) {
      return false;
    }
    // Otherwise, if *something* is a bigger dist than one away, keep moving closer!
    if (abs(cX-gX)>1 || abs(cY-gY)>1 || abs(cZ-gZ)>1) {
      tracker.changePosTowards(slotToPosition(gX,0),slotToPosition(gY,1),slotToPosition(gZ,2),tileSize/16.0); // was /4
    } else {
      // But if not, well, close enough! Next move would have been there anyways
        tracker.changePosTowards(slotToPosition(gX,0),slotToPosition(gY,1),slotToPosition(gZ,2),tileSize/32.0); // was /4
    }

    // Then Reset Cam Tracker Pos
    cX = getCollisionMapSlot(&tracker,0);
    cY = getCollisionMapSlot(&tracker,1);
    cZ = getCollisionMapSlot(&tracker,2);
  }
  return true;
}

// Check if line between two spots on grid is empty or full of invisible cubes only
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

// Same as above, only maps instead of arrays
bool checkPathVisibility(CubeObj* c2, CubeObj* target, Map* m) {
  // Used to check all spaces between cam and target,
  // it will follow a line from the cameraObj to the player within the perm map
  // checking each slot along that line, and returning false if there is an
  // occupation before the player is reached.
  
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
  CubeObj tracker;
  tracker.setPos(c->getX(), c->getY(), c->getZ());
  c->setLOS(checkPathVisibility(&tracker,c->getPermanentTarget(),m));
}

// Same as above, just maps instead of arrays
void checkCameraLOS(CameraObj* c, Map* m) {
  
  // Used to check all spaces between cam and target,
  // it will follow a line from the cameraObj to the player within the perm map
  // checking each slot along that line, and returning false if there is an
  // occupation before the player is reached.
  CubeObj tracker;
  tracker.setPos(c->getX(), c->getY(), c->getZ());
  c->setLOS(checkPathVisibility(&tracker,c->getPermanentTarget(),m));
}

// Tells Camera if it can see a specific cube or not, sets up Line of Sight
bool getCameraToCubeLOS(CameraObj* c, CubeObj* d, CubeObj* m[][maxHeight][maxDepth]) {

  // Used to check all spaces between cam and target,
  // it will follow a line from the cameraObj to the player within the perm map
  // checking each slot along that line, and returning false if there is an
  // occupation before the player is reached.

  CubeObj tracker;
  tracker.setPos(c->getX(), c->getY(), c->getZ());
  // FIXME: Feeling rusty, do I need to delete tracker again to avoid a mem leak, or is containment in function ample?
  
  return checkPathVisibility(&tracker,d,m);
}

// Same as above, but with maps instead of arrays
bool getCameraToCubeLOS(CameraObj* c, CubeObj* d, Map* m) {

  // Used to check all spaces between cam and target,
  // it will follow a line from the cameraObj to the player within the perm map
  // checking each slot along that line, and returning false if there is an
  // occupation before the player is reached.

  CubeObj tracker;
  tracker.setPos(c->getX(), c->getY(), c->getZ());
  // FIXME: Feeling rusty, do I need to delete tracker again to avoid a mem leak, or is containment in function ample?
  
  return checkPathVisibility(&tracker,d,m);
}

// No checkAndBounce if out of bounds
void tryCheckAndBounce(CubeObj* i, CubeObj* m[][maxHeight][maxDepth], int cX, int cY, int cZ) {
    if (cX >= 0 && cX < maxWidth && cY >= 0 && cY < maxHeight && cZ >=0 && cZ < maxDepth) {
        Collision::checkAndBounce(i,m[cX][cY][cZ]);
    }
}

// Same as above, but with maps instead of arrays
void tryCheckAndBounce(CubeObj* i, Map* m, int cX, int cY, int cZ) {
    if (cX >= 0 && cX < maxWidth && cY >= 0 && cY < maxHeight && cZ >=0 && cZ < maxDepth) {
        Collision::checkAndBounce(i,m->getCubeAt(cX,cY,cZ));
    }
}

/*******************
 * CAMERA COMMANDS *
 *******************/

void playerCenterCam(int i) {
  camera[i].setPlayerCenterCommand(true);
}
void playerLeftCam(int i) {
  camera[i].setPlayerLeftCommand(true);
}
void playerRightCam(int i) {
  camera[i].setPlayerRightCommand(true);
}
void playerUpCam(int i) {
  camera[i].setPlayerUpCommand(true);
}
void playerDownCam(int i) {
  camera[i].setPlayerDownCommand(true);
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

// Same as above, only with a map instead of an array
void explodingDiamondCollision(CubeObj* i, Map* m, int cX, int cY, int cZ) {
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

// Same as above, only with maps instead of arrays
void unintelligentCollision(CubeObj* i, Map* m, int cX, int cY, int cZ) {
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

// Same as above, only for maps instead of arrays
void addToCollisionMap(CubeObj* c1, Map* map) {
	int cX = getCollisionMapSlot(c1,0);
	int cY = getCollisionMapSlot(c1,1);
	int cZ = getCollisionMapSlot(c1,2);
  map->addCube(c1,cX,cY,cZ);
}

// Find all cubes surrounding one cube, and assign them to that cube as neighbors
void findNeighbors(CubeObj* c1, CubeObj* map[][maxHeight][maxDepth]) {
  int cX = getCollisionMapSlot(c1,0);
  int cY = getCollisionMapSlot(c1,1);
  int cZ = getCollisionMapSlot(c1,2);
  CubeObj* left  = map[cX+1][cY][cZ];
  CubeObj* right = map[cX-1][cY][cZ];
  CubeObj* up    = map[cX][cY+1][cZ];
  CubeObj* down  = map[cX][cY-1][cZ];
  CubeObj* front = map[cX][cY][cZ+1];
  CubeObj* back  = map[cX][cY][cZ-1];
  // The top/bot order on these might be wrong, but it shouldn't really matter too much
  // since used to check if surrounded on a plane anyways
  bool n0 = insideMap(cX+1, cY, cZ) && left != 0;
  bool n1 = insideMap(cX-1, cY, cZ) && right != 0;
  bool n2 = insideMap(cX, cY+1, cZ) && up != 0;
  bool n3 = insideMap(cX, cY-1, cZ) && down != 0;
  bool n4 = insideMap(cX, cY, cZ+1) && front != 0;
  bool n5 = insideMap(cX, cY, cZ-1) && back != 0;
  c1->setNeighbors(n0,n1,n2,n3,n4,n5);
  // Then add invisibility to the mix, and set visible neighbors
  if (n0) { n0 = n0 && !(left->isInvisible()); }
  if (n1) { n1 = n1 && !(right->isInvisible()); }
  if (n2) { n2 = n2 && !(up->isInvisible()); }
  if (n3) { n3 = n3 && !(down->isInvisible()); }
  if (n4) { n4 = n4 && !(front->isInvisible()); }
  if (n5) { n5 = n5 && !(back->isInvisible()); }
  c1->setVisibleNeighbors(n0,n1,n2,n3,n4,n5);
  c1->setVisibleNeighborObjects(left,right,up,down,front,back);
}

// Same as above, but with a map object
void findNeighbors(CubeObj* c1, Map* map) {
  int cX = getCollisionMapSlot(c1,0);
  int cY = getCollisionMapSlot(c1,1);
  int cZ = getCollisionMapSlot(c1,2);
  CubeObj* left  = map->getCubeAt(cX+1,cY,cZ);
  CubeObj* right = map->getCubeAt(cX-1,cY,cZ);
  CubeObj* up    = map->getCubeAt(cX,cY+1,cZ);
  CubeObj* down  = map->getCubeAt(cX,cY-1,cZ);
  CubeObj* front = map->getCubeAt(cX,cY,cZ+1);
  CubeObj* back  = map->getCubeAt(cX,cY,cZ-1);
  // The top/bot order on these might be wrong, but it shouldn't really matter too much
  // since used to check if surrounded on a plane anyways
  bool n0 = insideMap(cX+1, cY, cZ) && left != 0;
  bool n1 = insideMap(cX-1, cY, cZ) && right != 0;
  bool n2 = insideMap(cX, cY+1, cZ) && up != 0;
  bool n3 = insideMap(cX, cY-1, cZ) && down != 0;
  bool n4 = insideMap(cX, cY, cZ+1) && front != 0;
  bool n5 = insideMap(cX, cY, cZ-1) && back != 0;
  c1->setNeighbors(n0,n1,n2,n3,n4,n5);
  // Then add invisibility to the mix, and set visible neighbors
  if (n0) { n0 = n0 && !(left->isInvisible()); }
  if (n1) { n1 = n1 && !(right->isInvisible()); }
  if (n2) { n2 = n2 && !(up->isInvisible()); }
  if (n3) { n3 = n3 && !(down->isInvisible()); }
  if (n4) { n4 = n4 && !(front->isInvisible()); }
  if (n5) { n5 = n5 && !(back->isInvisible()); }
  c1->setVisibleNeighbors(n0,n1,n2,n3,n4,n5);
  c1->setVisibleNeighborObjects(left,right,up,down,front,back);
}

// If a position is not
bool insideMap(int a, int b, int c) {
  return !(a < 0 || b < 0 || c < 0 || 
          a >= maxWidth || b >= maxHeight || c >= maxDepth);
}

// Wipe current collision map to prep for repopulating it
void wipeCurrentMap(CubeObj* map[][maxHeight][maxDepth]){
  for (int a=0; a<currentMapWidth; a++) {
  for (int b=0; b<currentMapHeight;b++) {
  for (int c=0; c<currentMapDepth; c++) {
    if (map[a][b][c] != NULL) {
      // FIXME: Can't delete this? Memory leak caused because of it?
      //delete map[a][b][c];
      map[a][b][c] = NULL;
    }
  } } }
}

// Same as above, only maps instead of arrays
void wipeCurrentMap(Map* map){
  /*for (int a=0; a<currentMapWidth; a++) {
  for (int b=0; b<currentMapHeight;b++) {
  for (int c=0; c<currentMapDepth; c++) {
    if (map->getCubeAt(a,b,c) != NULL) {
      // FIXME: Can't delete this? Memory leak caused because of it?
      map->removeCubeAt(a,b,c);
      //map->addCube(NULL,a,b,c);
    }
  } } }*/

  map->wipeMap();
  
  /*
  #ifdef __APPLE_CC__
  #else 
  if (NULL != map) {
    // Alright, so… this breaks the game for Macs
    // so even though it looks like a memory leak otherwise,
    // to get the game functioning right now, we can only delete
    // the old map on Windows
    delete map;
    map = NULL;
  }
  #endif
  
  map = new Map;
  */
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
  return positionToSlot(cubePosition,d);
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

// pass position and dimension to get map slot
int positionToSlot(int a, int d) {
  int map = (d==0? currentMapWidth : d==1? currentMapHeight : currentMapDepth);
  int cubePosition = a;
  int cubeRadius = 50;//(c->getSize(d))/2; // FIXME: USING C->GETSIZE(D) HERE CAUSES A SEGFAULT >:( probably has to do with virtual functions
  int mapHalfSize = map/2*tileSize;
  int result = (cubePosition - cubeRadius + mapHalfSize)/tileSize;
  return result;
}
// Shorthand for getCollisionMapPosition
int slotToPosition(int slot, int d) {
  return getCollisionMapPosition(slot,d);
}

// Return if empty space here or not
bool freeSpaceAt(int a, int b, int c) {
  int slotA = positionToSlot(a,0);
  int slotB = positionToSlot(b,1);
  int slotC = positionToSlot(c,2);
  // Outside map bounds?
  if (slotA < 0 || slotB < 0 || slotC < 0 ||
      slotA >= getMapWidth() || slotB >= getMapHeight() || slotC >= getMapDepth()) {
        return true;
  } else {
  #ifdef newPermanentMap
    CubeObj* c1 = permanentMap->getCubeAt(slotA,slotB,slotC);
  #else
    CubeObj* c1 = permanentMap[slotA][slotB][slotC];
  #endif
    return (c1 == NULL || (c1->isInvisible()));
  }
}

// Returns gameplay state
CubiorObj* getPlayer() { return &cubior[0]; }
CubiorObj* getPlayer(int i) { return &cubior[i]; }
CubeObj* getCube() { return cube[0]; }
CubeObj* getCube(int i) { return cube[i]; }
GoalObj* getGoal() { return &goal; }
// Total Cubiors that can be played
const int getCubiorCount() { return cubiorCount; }
bool getCubiorPlaying(int i) { return cubiorPlaying[i]; }
bool getCubiorOnline(int i) { return cubiorOnline[i]; }
bool getCubiorLocal(int i) { return cubiorPlaying[i] && !cubiorOnline[i]; }

// How many are in play total
int getCubiorsPlaying() {
  int results = 0;
  for (int i=0; i<cubiorCount; i++) {
    if (getCubiorPlaying(i)) { results++; }
  }
  return results;
}
// On in play locally
int getLocalCubiorsPlaying() {
    int results = 0;
    for (int i=0; i<cubiorCount; i++) {
        if (getCubiorLocal(i)) { results++; }
    }
    return results;
}
// How many are online total
int getCubiorsOnline() {
    int results = 0;
    for (int i=0; i<cubiorCount; i++) {
        if (cubiorOnline[i]) { results++; }
    }
    return results;
}

// Find if player is add-able
int getNewPlayer() {
  for (int i=0; i<cubiorCount; i++) {
    if (!cubiorPlaying[i]) {
      return i;
    }
  }
  return -1; 
}

// Find if player is add-able
int getNewLocalPlayer() {
    for (int i=0; i<cubiorCount; i++) {
        if (!cubiorPlaying[i] && !cubiorOnline[i]) {
            return i;
        }
    }
    return -1;
}
int addPlayer(bool online) {
    // Add first player you can find
    for (int i=0; i<cubiorCount; i++) {
        if (!cubiorPlaying[i] && !cubiorOnline[i]) {
            setCubiorPlaying(i, true);
            setCubiorOnline(i, online);
            return 0;
        }
    }
    // Couldn't find an empty player slot, game is full
    return 1;
}

int addPlayer(int i, bool online) {
  if (!cubiorPlaying[i]) {
    setCubiorPlaying(i, true);
    setCubiorOnline(i, online);
    return 0;
  }
  return 1;
};

int removePlayer(int i) {
    if (cubiorPlaying[i]) {
        setCubiorPlaying(i, false);
        setCubiorOnline(i, false);
        return 0;
    } else {
        // Player not available to be removed
        return 1;
    }
}

void setCubiorPlaying(int i, bool b) {
    resetCubior(i);
    cubiorPlaying[i] = b;
}
void setLocalCubiorPlaying(int i, bool b) {
    if (!cubiorOnline[i]) {
        resetCubior(i);
        cubiorPlaying[i] = b;
    }
}
void setCubiorOnline(int i, bool b) {
    cubiorOnline[i] = b;
    cubior[i].setOnline(b);
}
    
// Sfx triggers
bool getCubiorJustJumped(int i) { return cubior[i].justJumped(); }
bool getCubiorJustBumped(int i) { return cubior[i].justBumped(); }
int getCubiorJustLanded(int i) { return cubior[i].justLanded(); }
int getCubiorJustSkidded(int i) { return cubior[i].justSkidded(); }
int getCubiorJustFlipped(int i) { return cubior[i].justFlipped(); }
int getCubiorJustMoved(int i) { return cubior[i].justMoved(); }
bool getJustExited() { return justExited; }
bool getJustPaused() { return justPaused; }
bool getJustUnpaused() { return justUnpaused; }
bool getJustChangedMenu() { return justChangedMenu; }
bool getJustChangedOption() { return justChangedOption; }
bool getJustCausedError() { return justCausedError; }
bool getJustFocusedCamera() { return justFocusedCamera; }
bool getJustTurnedCamera() { return justTurnedCamera; }
bool getJustFailedCamera() { return justFailedCamera; }
void setJustExited(bool b) { justExited = b; }
void setJustPaused(bool b) { justPaused = b; }
void setJustUnpaused(bool b) { justUnpaused = b; }
void setJustChangedMenu(bool b) { justChangedMenu = b; }
void setJustChangedOption(bool b) { justChangedOption = b; }
void setJustCausedError(bool b) { justCausedError = b; }
void setJustFocusedCamera(bool b) { justFocusedCamera = b; }
void setJustTurnedCamera(bool b) { justTurnedCamera = b; }
void setJustFailedCamera(bool b) { justFailedCamera = b; }

const int getMaxCubeCount() { return maxCubeCount; }
int getCubeCount() { return cubeCount; }
CameraObj* getCamera() { return &camera[0]; }
CameraObj* getCamera(int i) { return &camera[i]; }

bool getInvincibility(int n) { return cubior[n].getInvincibility(); }
void setInvincibility(int n, bool newState) { cubior[n].setInvincibility(newState); }

int getGravity() { return gravity; }

void  enableGoodCollision() { goodCollision = true;  }
void disableGoodCollision() { goodCollision = false; }
void  stopGameplay(int i) { gameplayRunning = false; resetOption(i); }
void startGameplay() { gameplayRunning = true;  }
bool getGameplayRunning() { return gameplayRunning; }

// Pause option values for when gameplay is not running
void resetOption(int i) { option[i] = 0; }
void nextOption(int i) {
  if (maxOption[menu[i]] > 1) { justChangedOption = true; }
  option[i] = (option[i]+1) % maxOption[menu[i]];
}
void prevOption(int i) {
  if (maxOption[menu[i]] > 1) { justChangedOption = true; }
  option[i] = (option[i]+maxOption[menu[i]]-1) % maxOption[menu[i]];
}
int getOption(int i) { return option[i]; }
// A more tricky function, activate your selection
void chooseOption(int i) {
  if (menu[i] == 0) { // Splash Screen
    switch(option[i]) {
      default:
        // Switch to Start Screen only with start/enter, not choose/A/Space
        break;
    }
  } else if (menu[i] == 1) { // Start Menu (Main Menu)
    switch(option[i]) {
      case 0:
        // See Controller Map Just Before Starting Gameplay
        justHitStart = true;
        setMenu(i, 4);//menu[controller] = 4;
        break;
      case 1:
        // Change options
        setMenu(i, 3);//menu[0] = 3;
        break;
      case 2:
        // Play credits
        setMenu(i, 5);//menu[0] = 5;
        break;
      case 3:
        // Quit game
        exit(0);
        break;
      default:
        break;
    }
  } else if (menu[i] == 2) { // Pause Menu
    switch(option[i]) {
      case 0:
        // Resume game
        startGameplay();
        setJustUnpaused(true);
        break;
      case 1:
        // Drop out
        if (getCubiorsPlaying() > 1) {
          startGameplay();
          setJustUnpaused(true);
          setCubiorPlaying(i,false);
          resetControlsPlayer(i);
        } else {
          justCausedError = true;
        }
        break;
      case 2:
        // Change options
        setMenu(i, 3);//menu[i] = 3;
        break;
      case 3:
        // Return to Title Screen
        restartGame(i);
        break;
      default:
        break;
    }
  } else if (menu[i] == 3) { // Options Menu
    switch(option[i]) {
      case 0:
        // Toggle Angle Controls
        toggleIndependentMovement(i);
        break;
      case 1:
        // See Controller Map
        setMenu(i, 4);//menu[controller] = 4;
        break;
      case 2:
        // Enter Volume Settings
        setMenu(i, 6);
        break;
      case 3:
        // Return to pause or start menu
        if (getLastPause() == -1) { // start menu
          setMenu(i, 1);//menu[controller] = 1;
        } else {                    // pause menu
          setMenu(i, 2);//menu[controller] = 2;
        }
        break;
      default:
        break;
    }
  } else if (menu[i] == 4) { // Controller Map
    switch(option[i]) {
      case 0:
        if (!justHitStart) {
          // Return to options menu
          setMenu(i, 3);//menu[controller] = 3;
          break;
        } else {
          // Start game
          startGameplay();
          setJustUnpaused(true);
          // Change all players to pause menu
          for (int j=0; j<4; j++) {
            setMenu(j,2);
          }
          justHitStart = false;
          break;
        }
      default:
        break;
    }
  } else if (menu[i] == 5) { // Credits
    switch(option[i]) {
      case 0:
        // Return to start menu
        setMenu(i, 1);//menu[controller] = 1;
        break;
      default:
        break;
    }
  } else if (menu[i] == 6) { // Volume Settings
    switch(option[i]) {
      case 0:
        // Toggle Volume Settings
        cycleMusicVolume();
        break;
      case 1:
        // Toggle Volume Settings
        cycleSoundVolume();
        break;
      case 2:
        // Return to options menu
        setMenu(i, 3);//menu[controller] = 1;
        break;
      default:
        break;
    }
  }

  // And if no errors
  if (!justCausedError) {
    // selection gets a menu selection sound trigger
    justChangedMenu = true;
  }
}

bool getStarted() { return started; }
void setStarted(bool b) { started = b; }
    
int getMenu() { return menu[0]; }
int getMenu(int i) { return menu[i]; }
void setMenu(int i, int j) {
  // Any menu selection also gets a menu selection sound trigger
  justChangedMenu = true;

  menu[i] = j;
  option[i] = 0;
}

int getMapWidth()   { if (levelMap != NULL) { return levelMap->getWidth() ; } else { return 0; }}
int getMapHeight()  { if (levelMap != NULL) { return levelMap->getHeight(); } else { return 0; }}
int getMapDepth()   { if (levelMap != NULL) { return levelMap->getDepth() ; } else { return 0; }}
float getMapRed()   { return levelMap->getRed();  }
float getMapGreen() { return levelMap->getGreen();}
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

void switchFullscreen() {
  toggleFullscreen();
}

void switchLevelShadows() {
  toggleLevelShadows();
  // HIJACKED! Used for songs atm for Rolando demo
  //nextSong();
  // No use in demo form
}

// Find if cube n is the last down (no shadow)
bool getShadow(int i) {
  int slotY = getCollisionMapSlot(cube[i],1);
  // no shadow if 0th spot or neighbor beneath
  if (slotY!=0 && !((cube[i]->getNeighbors())[3])) {//permanentMap[slotX][slotY-1][slotZ] == NULL) {
    int slotX = getCollisionMapSlot(cube[i],0);
    int slotZ = getCollisionMapSlot(cube[i],2);
    // Check all inbetween slots
    for (int j=slotY-2; j>=0; j--) {
    #ifdef newPermanentMap
      if (permanentMap->getCubeAt(slotX,j,slotZ) != NULL) {
    #else
      if (permanentMap[slotX][j][slotZ] != NULL) {
    #endif
      // Found somebody else! You are NOT the last cube down
        return true;
      }      
    }
  }
  // So no other cubes were found!
  return false;
}

// give if camera locked in any way
bool getCameraLocked(int i) {
  return camera[i].getLockedToPlayer()
      || camera[i].getLockedToPlayerX()
      || camera[i].getLockedToPlayerZ();
}

// Take an angle and lock it to a 45 degree mark
int snapLockAngle(int i) {
  int lockedAngleY = i;
  while (lockedAngleY < 0) { lockedAngleY += 360; }
  int distFromLock = (int)(lockedAngleY) % 45;
  int extraOne = (distFromLock > 22 ? 1 : 0);
  lockedAngleY = (floor((lockedAngleY - distFromLock) / 45.0) + extraOne) * 45.0;
  return lockedAngleY;
}

// How much to move compared to how fast game
float fpsRate() {
  // pass it along from flatRender
  return getFPSRate();
}

// Give flatrender info on whether controls are displayed for options or for starting
bool getJustHitStart() { return justHitStart; }
