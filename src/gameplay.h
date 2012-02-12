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
    void fall();
    bool playerMoving();
    void freezePlayer();
    void setPlayerX(int);
    void setPlayerY(int);
    void setPlayerZ(int);
    void setPlayerAngleZ(int);
    void changePlayerX(int);
    void changePlayerY(int);
    void changePlayerZ(int);
    void changePlayerAngleZ(int);
    void movePlayerX(int);
    void movePlayerY(int);
    void movePlayerZ(int);
    void movePlayerAngleZ(int);
    void jump(bool);

    CubeObj* getPlayer();
    int getPlayerX();
    int getPlayerY();
    int getPlayerZ();
    int getPlayerAngleZ();
    bool getLocking();
    void setLocking(bool);
    bool getInvincibility();
    void setInvincibility(bool);
    float getHappiness();

    int getFloor();
    int getGravity();

#endif 
