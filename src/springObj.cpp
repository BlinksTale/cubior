//
//  springObj.cpp
//  Cubior
//
//  Created by Brian Handy on 4/8/14.
//
//

#include "springObj.h"
#include "cubiorObj.h"
#include "springShape.h"

int springOffsetY = -80;

SpringObj::SpringObj() {
    permalocked = true;
    // something like this? Probably not. this->setMaterial(9);
    item = true;
    itemType = "spring";
}

void SpringObj::collisionEffect(CubeObj* c) {
    if (c->isPlayer()) {
        // if it was a nonsolid, which we haven't implemented,
        // (... && c->getY() <= this->getY() + 50) would allow for
        // standard positions rather than offset by -80
        
        // also for the condition:
        // FIXME: add a && c->getMomentumY() < springMomentum/2f
        
        c->setMomentumY(springMomentum);
    }
}
void SpringObj::setX(int n) { x = n; }
void SpringObj::setY(int n) { y = n + springOffsetY; }
void SpringObj::setZ(int n) { z = n; }
void SpringObj::setPos(int n, int o, int p) { x = n, y = o + springOffsetY, z = p; }