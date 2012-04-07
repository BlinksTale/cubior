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
const int playableWidth = 30;
const int playableHeight= 20;
const int playableDepth = 30;
const int mapWidth = playableWidth + mapEdge*2;
const int mapHeight= playableHeight + mapEdge*2;
const int mapDepth = playableDepth + mapEdge*2;
const int tileSize = 100;

const int cubiorCount = 4;
const int cubeCount = 6 + (playableWidth+1)*(playableDepth+1);

    void gameplayStart();
    void gameplayLoop();

    void explodingDiamondCollision(CubeObj*,CubeObj*[][mapHeight][mapDepth],int,int,int);
    void unintelligentCollision(CubeObj*,CubeObj*[][mapHeight][mapDepth],int,int,int);
    void addToCollisionMap(CubeObj*,CubeObj*[][mapHeight][mapDepth]);
    void findNeighbors(CubeObj*,CubeObj*[][mapHeight][mapDepth]);
    bool* getNeighbors(CubeObj*);
    void wipeMap(CubeObj*[][mapHeight][mapDepth]);
    int getCollisionMapSlot(CubeObj*,int);
    CubiorObj* getPlayer();
    CubiorObj* getPlayer(int);
    CubeObj* getCube();
    GoalObj* getGoal();
    CubeObj* getCube(int);
    const int getCubiorCount();
    bool getCubiorPlayable(int);
    void setCubiorPlayable(int,bool);
    const int getCubeCount();
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
