/*
 * Gameplay header for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */
#ifndef GAMEPLAY
#define GAMEPLAY

    void gameplayLoop();
    void setPlayerX(int);
    void setPlayerY(int);
    void setPlayerAngleZ(int);
    void changePlayerX(int);
    void changePlayerY(int);
    void changePlayerAngleZ(int);
    void movePlayerX(int);
    void movePlayerY(int);
    void movePlayerAngleZ(int);
    int getPlayerX();
    int getPlayerY();
    int getPlayerAngleZ();

#endif 
