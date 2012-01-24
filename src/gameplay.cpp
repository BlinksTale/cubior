/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */
#include "gameplay.h"

float x = -9;
float y = -9;
static float movementSpeed = 0.25;

void gameplayLoop() {
  // increase player position
  x += movementSpeed;
  y += movementSpeed;
}

// Returns gameplay state
float getPlayerX() {
  return x;
}
float getPlayerY() {
  return y;
}
