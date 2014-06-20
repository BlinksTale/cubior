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
}

void CrumblingObj::collisionEffect(CubeObj* c) {
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