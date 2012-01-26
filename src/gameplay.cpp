/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */
#include "gameplay.h"

int x = -9;
int y = -9;
int angleZ = 0;
static int movementSpeed = 10;
static int rotationSpeed = 10;

void gameplayLoop() {
  // increase player position
  angleZ += rotationSpeed / 10;
  //y += movementSpeed;
}

// Sets gameplay state
void setPlayerX(int newX) { x = newX; }
void setPlayerY(int newY) { y = newY; }
void setPlayerAngleZ(int newAngleZ) { angleZ = newAngleZ; }
void changePlayerX(int newX) { x += newX; }
void changePlayerY(int newY) { y += newY; }
void changePlayerAngleZ(int newAngleZ) { angleZ += newAngleZ; }
void movePlayerX(int newX) { x += newX * movementSpeed; }
void movePlayerY(int newY) { y += newY * movementSpeed; }
void movePlayerAngleZ(int newAngleZ) { angleZ += newAngleZ * rotationSpeed; }

// Returns gameplay state
int getPlayerX() { return x; }
int getPlayerY() { return y; }
int getPlayerAngleZ() { return angleZ; }
