/*
 * cubiorObj.cpp
 * by Brian Handy
 * 2/12/12
 * Represents any cubior in the game (npcs and players)
 */

#include "cubeObj.h"
#include "gameplay.h"
#include "cubiorObj.h"

CubiorObj::CubiorObj() {
  happiness = 1.0;
  invincible = false;
  playerStatus = true;
  
}

void CubiorObj::setHappiness(float n) {
  happiness = n;
}

void CubiorObj::setInvincibility(bool n) {
  invincible = n;
}

int CubiorObj::getWidth() { return 100*(1+locked*1); }
int CubiorObj::getHeight() { return 100*(1-locked*1); }
