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
    // Everything is in itemShape
public:
    void initVisuals();
    void draw();
};

#endif /* defined(__Cubior__crumblingShape__) */
