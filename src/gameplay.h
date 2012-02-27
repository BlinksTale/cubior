/*
 * Gameplay header for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#ifndef GAMEPLAY
#define GAMEPLAY

#include "cubeObj.h"
#include "cubiorObj.h"
#include "cubiorObj.h"

    void gameplayStart();
    void gameplayLoop();

    CubiorObj* getPlayer();
    CubiorObj* getPlayer(int);
    CubeObj* getCube();
    CubeObj* getCube(int);
    int getCubiorCount();
    int getCubeCount();

    bool getInvincibility(int);
    void setInvincibility(int, bool);

    int getFloor();
    int getGravity();

    bool collision(CubeObj*,CubeObj*);
    void bounce(CubeObj*,CubeObj*);
    void bouncePrecisely(CubeObj*,CubeObj*);
    void balanceMomentum(CubeObj*,CubeObj*);

#endif 
