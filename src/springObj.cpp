//
//  springObj.cpp
//  Cubior
//
//  Created by Brian Handy on 4/8/14.
//
//

#include "springObj.h"
#include "cubiorObj.h"

void SpringObj::collisionEffect(CubeObj* c) {
    if (c->isPlayer()) { // FIXME: add a && c->getMomentumY() < springMomentum/2f
        c->setMomentumY(springMomentum);
    }
}