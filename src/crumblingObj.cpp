//
//  crumblingObj.cpp
//  Cubior
//
//  Created by Brian Handy on 5/10/14.
//
//

#include "crumblingObj.h"
#include "cubiorObj.h"

CrumblingObj::CrumblingObj() {
    super::init();
    itemType = "crumbling";
    timeSinceCollisionMax = 3.0;
    timeCollidingMax = 0.2;
}

void CrumblingObj::tick() {
  super::tick();
  
  if (!justHitPlayer && timeSinceCollision >= timeSinceCollisionMax) {
    if (!solid)
      solid = true;
    if (timeColliding > 0.0f)
      timeColliding -= 1.0f/60.0f;
    else if (timeColliding != 0.0f)
      timeColliding = 0.0f;
  }
}

void CrumblingObj::collisionEffect(CubeObj* c) {
    if (c->isPlayer()) {
      // Again, switch this to delta time (fixme)
      if (timeColliding < timeCollidingMax)
        timeColliding += 1.0f/60.0f;
      else
        timeColliding = timeCollidingMax;
      
      if (timeColliding >= timeCollidingMax)
        solid = false;
    }
    super::collisionEffect(c);
}