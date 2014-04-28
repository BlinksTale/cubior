//
//  springShape.h
//  Cubior
//
//  Created by Brian Handy on 4/12/14.
//
//

#ifndef __Cubior__springShape__
#define __Cubior__springShape__

#include "cubeShape.h"
#include "cubeObj.h"
#include <iostream>

class SpringShape : public CubeShape {
private:
    typedef CubeShape super;
protected:
    //float r1,g1,b1,r2,g2,b2;
    bool glow;
    
    float colorDarkness;
    float colorDefaultA;
    float colorDefaultR;
    float colorDefaultG;
    float colorDefaultB;
    float colorCurrentR;
    float colorCurrentG;
    float colorCurrentB;
    
public:
    void initVisuals();
    void updateVisuals();
    void updateColors();
    void draw();
};

#endif /* defined(__Cubior__springShape__) */
