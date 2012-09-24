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
}

void GoalObj::collisionEffect(CubeObj* c) {
  if (c->isPlayer()) {
    nextLevel();
  }
}
