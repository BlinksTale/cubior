/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */
#include "gameplay.h"

int x = -39;
int y = -39;

void gameplayLoop() {
  // increase player position
  x++;
  y++;
}

// Returns gameplay state
int getPlayerX() {
  return x;
}
int getPlayerY() {
  return y;
}
