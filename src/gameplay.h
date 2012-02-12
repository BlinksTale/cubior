/*
 * Gameplay header for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#ifndef GAMEPLAY
#define GAMEPLAY

#include "cubeObj.h"

    void gameplayStart();
    void gameplayLoop();

    CubeObj* getPlayer();

    bool getInvincibility();
    void setInvincibility(bool);
    float getHappiness();

    int getFloor();
    int getGravity();

#endif 
