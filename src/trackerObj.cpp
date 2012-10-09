/*
 * trackerObj.cpp
 * by Brian Handy
 * 4/24/12
 * Trackers, for following something - extends CubeObj
 */

#include "cubeObj.h"
#include "trackerObj.h"
#include <math.h>
// print stuff!
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
// that's it for printing


TrackerObj::TrackerObj() {
  //target = NULL;
}

void TrackerObj::tick() {
  if (target) {
    int xDiff = target->getX()-x;
    int zDiff = target->getZ()-z;
    int distToTarget = sqrt((double)(xDiff*xDiff+zDiff*zDiff));
    chasing = distToTarget > followDist;
    if (chasing) {
      x+=(xDiff*(distToTarget-followDist)/distToTarget);
      z+=(zDiff*(distToTarget-followDist)/distToTarget);
    }
    calculateDiff();
    if (target->getGrounded()) { y = target->getY(); }
  }
}

bool TrackerObj::getChasing() { return chasing; }
void TrackerObj::setTarget(CubeObj* n) {
  target = n;
}
