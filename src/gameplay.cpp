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
static int movementSpeed = 1;

void gameplayLoop() {
  // increase player position
  x += movementSpeed;
  //y += movementSpeed;
}

// Returns gameplay state
int getPlayerX() {
  return x;
}
int getPlayerY() {
  return y;
}
int getPlayerAngleZ() {
  return angleZ;
}
