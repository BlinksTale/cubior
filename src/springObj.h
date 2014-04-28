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
private:
    typedef CubeObj super;

    const float springMomentum = 1000.0f;
    void collisionEffect(CubeObj*);
    
public:
    SpringObj();
    
    void setX(int);
    void setY(int);
    void setZ(int);
    void setPos(int,int,int);

};

#endif /* defined(__Cubior__springObj__) */
