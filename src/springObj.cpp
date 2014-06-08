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

const float springMomentum = 1000.0f;

int springOffsetY = -80;

SpringObj::SpringObj() {
    super::init();
    itemType = "spring";
}

void SpringObj::collisionEffect(CubeObj* c) {
    if (c->isPlayer()) {
        // if it was a nonsolid, which we haven't implemented,
        // (... && c->getY() <= this->getY() + 50) would allow for
        // standard positions rather than offset by -80
        
        // also for the condition:
        // FIXME: add a && c->getMomentumY() < springMomentum/2f
        
        //float negative = -1 * c->getMomentumY();
        //if (c->getMomentumY() > 0)
        c->setMomentumY(springMomentum); // springMomentum
    }
    super::collisionEffect(c);
}
void SpringObj::setX(int n) { x = n; }
void SpringObj::setY(int n) { y = n + springOffsetY; }
void SpringObj::setZ(int n) { z = n; }
void SpringObj::setPos(int n, int o, int p) { x = n, y = o + springOffsetY, z = p; }