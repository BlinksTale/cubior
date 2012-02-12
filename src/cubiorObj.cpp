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
  
}

void CubiorObj::setHappiness(float n) {
  happiness = n;
}

void CubiorObj::setInvincibility(bool n) {
  invincible = n;
}
