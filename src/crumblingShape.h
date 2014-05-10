//
//  crumblingShape.h
//  Cubior
//
//  Created by Brian Handy on 5/10/14.
//
//

#ifndef __Cubior__crumblingShape__
#define __Cubior__crumblingShape__

#include <iostream>
#include "itemShape.h"
#include "cubeObj.h"

class CrumblingShape : public ItemShape {
private:
    typedef ItemShape super;
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
    void initVisuals();
    void updateVisuals();
    void draw();
};

#endif /* defined(__Cubior__crumblingShape__) */
