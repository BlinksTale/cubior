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

const int mapEdge = 3;

const int playableWidth = 40;
const int playableHeight= 20;
const int playableDepth = 40;
const int maxWidth = playableWidth + mapEdge*2;
const int maxHeight= playableHeight + mapEdge*2;
const int maxDepth = playableDepth + mapEdge*2;
const int tileSize = 100;

const int cubiorCount = 4;
const int maxCubeCount = 6 + (playableWidth+1)*(playableDepth+1);

    void gameplayStart();
    void gameplayLoop();

    void explodingDiamondCollision(CubeObj*,CubeObj*[][maxHeight][maxDepth],int,int,int);
    void unintelligentCollision(CubeObj*,CubeObj*[][maxHeight][maxDepth],int,int,int);
    void addToCollisionMap(CubeObj*,CubeObj*[][maxHeight][maxDepth]);
    void findNeighbors(CubeObj*,CubeObj*[][maxHeight][maxDepth]);
    bool* getNeighbors(CubeObj*);
    void wipeMap(CubeObj*[][maxHeight][maxDepth]);
    int getCollisionMapSlot(CubeObj*,int);
    CubiorObj* getPlayer();
    CubiorObj* getPlayer(int);
    CubeObj* getCube();
    GoalObj* getGoal();
    CubeObj* getCube(int);
    const int getCubiorCount();
    bool getCubiorPlayable(int);
    void setCubiorPlayable(int,bool);
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

#endif 
