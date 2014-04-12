//
//  springObj.h
//  Cubior
//
//  Created by Brian Handy on 4/8/14.
//
//

#ifndef __Cubior__springObj__
#define __Cubior__springObj__

#include "cubeObj.h"
#include <iostream>


class SpringObj : public CubeObj {
    
    const float springMomentum = 1000.0f;
    void collisionEffect(CubeObj*);
    
public:
    SpringObj();

};

#endif /* defined(__Cubior__springObj__) */
