/*
 * goalObj.cpp
 * by Brian Handy
 * 3/7/12
 * Represents the goal for each level
 */

#include "cubeObj.h"
#include "goalObj.h"
#include "gameplay.h"
#include <iostream>

using namespace std;

GoalObj::GoalObj() {
  permalocked = true;
  glow = false; // will glow when hit
  glowCount = 0;
  lastGlow = glow;
}

void GoalObj::collisionEffect(CubeObj* c) {
  if (c->isPlayer()) {
    // Make it white!
    glow = true;
    
    // Then start player anim sequence
    CubiorObj* temp = (CubiorObj*)c;
    int theNum = temp->getCubiorNum();
    nextLevelCountdown(theNum);

    // And play sound
    setJustExited(true);
  }
}

bool GoalObj::getGlow() { return glow; }
void GoalObj::setGlow(bool b) { glow = b; }

// If glowing, increment until glowMax is reached
// return true if it has been reached
bool GoalObj::getGlowMax() {
  if (glow) {
    if (glowCount == glowMax) {
      return true;
    } else {
      glowCount++;
    }
  }
  return false;
}

// Returns what it was last time this got updated
bool GoalObj::getLastGlow() {
  bool result = lastGlow;
  // Update lastGlow if needed
  if (lastGlow != glow) {
    lastGlow = glow;
  }
  return result;
}