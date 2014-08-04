//
//  itemObj.cpp
//  Cubior
//
//  Created by Brian Handy on 5/10/14.
//
//

#include "itemObj.h"
#include "cubiorObj.h"

ItemObj::ItemObj() {
    init();
}

void ItemObj::init() {
    permalocked = true;
    // something like this? Probably not. this->setMaterial(9);
    item = true;
    itemType = "undefinedItem";
  
    timeSinceCollisionMax = 0.15f;
  
    playerStatus = false;
}

void ItemObj::tick() {
  // If hit player this turn, reset timeSinceCollision
  // but if usingPlayerAbove, only activates if justHit from above
  if (justHitPlayer && (!usingPlayerAbove || playerAbove)) {
    timeSinceCollision = 0.0f;
  }
  
  super::tick();
  
  
}

void ItemObj::collisionEffect(CubeObj* c) {
    if (c->isPlayer()) {
        // if it was a nonsolid, which we haven't implemented,
        // (... && c->getY() <= this->getY() + 50) would allow for
        // standard positions rather than offset by -80
        
        // also for the condition:
        // FIXME: add a && c->getMomentumY() < springMomentum/2f
        
        //float negative = -1 * c->getMomentumY();
        //if (c->getMomentumY() > 0)
        //c->setMomentumY(springMomentum); // springMomentum
    }
    super::collisionEffect(c);
}
void ItemObj::setX(int n) { x = n; }
void ItemObj::setY(int n) { y = n; }
void ItemObj::setZ(int n) { z = n; }
void ItemObj::setPos(int n, int o, int p) { x = n, y = o, z = p; }