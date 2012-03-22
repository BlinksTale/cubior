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

    void gameplayStart();
    void gameplayLoop();

    void explodingDiamondCollision(CubeObj*,CubeObj*[][16][16],int,int,int);
    void unintelligentCollision(CubeObj*,CubeObj*[][16][16],int,int,int);
    void addToCollisionMap(CubeObj*,CubeObj*[][16][16]);
    void wipeMap(CubeObj*[][16][16]);
    int getCollisionMapSlot(CubeObj*,int);
    CubiorObj* getPlayer();
    CubiorObj* getPlayer(int);
    CubeObj* getCube();
    GoalObj* getGoal();
    CubeObj* getCube(int);
    const int getCubiorCount();
    const int getCubeCount();

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
