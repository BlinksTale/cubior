//
//  springShape.h
//  Cubior
//
//  Created by Brian Handy on 4/12/14.
//
//

#ifndef __Cubior__springShape__
#define __Cubior__springShape__

#include "itemShape.h"
#include "cubeObj.h"
#include <iostream>

class SpringShape : public ItemShape {
private:
    typedef ItemShape super;
protected:
public:
    void initVisuals();
    void draw();
};

#endif /* defined(__Cubior__springShape__) */
