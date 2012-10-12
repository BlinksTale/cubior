 /*
 * Gameplay header for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#ifndef GAMEPLAY
#define GAMEPLAY

#include "cubeObj.h"
#include "goalObj.h"
#include "cubiorObj.h"
#include "cameraObj.h"
#include <iostream> // necessary for string

using namespace std; // necessary for string

const bool rotateIfInvisible = true;
const int mapEdge = 4;
// How many angles to try when rotating back into view of player
const float anglesToTry = 8;
const int wallCheckRadius = 3;

const int playableWidth = 100;
const int playableHeight= 100;
const int playableDepth = 100;
const int maxWidth = playableWidth + mapEdge*2;
const int maxHeight= playableHeight + mapEdge*2;
const int maxDepth = playableDepth + mapEdge*2;
const int tileSize = 100;
const int totalLevels = 20; // for now, update this when new levels are added

const int cubiorCount = 4;
const int maxCubeCount = (playableHeight)*(playableWidth)*(playableDepth);

// Variables for how long we've moved towards intended, yet target is visible..
const int visibleIntendedMax = 0;
// longest period we can not see player w/o fixing it
const int invisibleMax = 5;

    void gameplayStart(string);
    void nextLevelCountdown(int);
    void nextLevel();
    void gameplayLoop();

    // Looks for vertical walls or clearings along 1 dimension of player
    int* searchForWall(int,int [],CubeObj* [][maxHeight][maxDepth],int);
    void rotateToAngle(int,float,int);
    bool isVertSpace(CubeObj* [][maxHeight][maxDepth], int, int, int);
    bool insideMap(int,int,int); // lets you know if pos is OK for Macs
    
    // Check LOS between cam and player, then move to clear shot by a clear path
    void ensurePlayerVisible(int);
    bool playerVisible(int);
    bool cubeVisible(int,int);
    void fixPlayerVisibility(int);
    void moveToPlayer(int);
    void rotateToPlayer(int);
    void rotateAroundPlayer(int,int); // for victory shot
    
    // Camera
    bool checkSlotPathVisibility(int,int,int,int,int,int, CubeObj* [][maxHeight][maxDepth]);
    bool checkPathVisibility(CubeObj*, CubeObj*, CubeObj* [][maxHeight][maxDepth]);
    void checkCameraLOS(CameraObj*, CubeObj*[][maxHeight][maxDepth]);
    bool getCameraToCubeLOS(CameraObj*, CubeObj*, CubeObj*[][maxHeight][maxDepth]);
    
    // Collision
    void explodingDiamondCollision(CubeObj*,CubeObj*[][maxHeight][maxDepth],int,int,int);
    void unintelligentCollision(CubeObj*,CubeObj*[][maxHeight][maxDepth],int,int,int);
    void addToCollisionMap(CubeObj*,CubeObj*[][maxHeight][maxDepth]);
    void findNeighbors(CubeObj*,CubeObj*[][maxHeight][maxDepth]);
    bool* getNeighbors(CubeObj*);
    void wipeCurrentMap(CubeObj*[][maxHeight][maxDepth]);
    void wipeFullMap(CubeObj*[][maxHeight][maxDepth]);
    int getCollisionMapSlot(CubeObj*,int);
    int getCollisionMapPosition(int,int);    
    int positionToSlot(CubeObj*,int);
    int slotToPosition(int, int);
  
    CubiorObj* getPlayer();
    CubiorObj* getPlayer(int);
    CubeObj* getCube();
    GoalObj* getGoal();
    CubeObj* getCube(int);
    const int getCubiorCount();
    bool getCubiorPlayable(int);
    void setCubiorPlayable(int,bool);
    
    // Sfx triggers
    bool getCubiorJustJumped(int);
    bool getCubiorJustBumped(int);
    bool getJustExited();
    bool getJustPaused();
    bool getJustUnpaused();
    // And setters for sfx triggers
    void setJustExited(bool);
    void setJustPaused(bool);
    void setJustUnpaused(bool);
    
    const int getMaxCubeCount();
    int getCubeCount();
    CameraObj* getCamera();
    CameraObj* getCamera(int);

    bool getInvincibility(int);
    void setInvincibility(int, bool);

    int getFloor();
    int getGravity();
    void enableGoodCollision();
    void disableGoodCollision();

    void stopGameplay();
    void startGameplay();
    bool getGameplayRunning();
    float getMapRed();
    float getMapGreen();
    float getMapBlue();
    
    float getAngleBetween(int,int,int,int);
    void switchFullscreen();
    void switchLevelShadows();
    
    bool getShadow(int);
    
#endif 
