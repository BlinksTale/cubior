//
//  itemShape.h
//  Cubior
//
//  Created by Brian Handy on 5/10/14.
//
//

#ifndef __Cubior__itemShape__
#define __Cubior__itemShape__

#include <iostream>
#include "cubeShape.h"
#include "cubeObj.h"

class ItemShape : public CubeShape {
private:
    typedef CubeShape super;
protected:
    float colorDarkness;
    float colorDefaultA;
    float colorDefaultR;
    float colorDefaultG;
    float colorDefaultB;
    float colorCurrentR;
    float colorCurrentG;
    float colorCurrentB;
public:
    void initVisuals(float,float,float,float,float,float,float,bool,bool);
    void updateVisuals();
    void updateColors();
    void draw();
};

#endif /* defined(__Cubior__itemShape__) */
