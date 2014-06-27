//
//  crumblingObj.h
//  Cubior
//
//  Created by Brian Handy on 5/10/14.
//
//

#ifndef __Cubior__cumblingObj__
#define __Cubior__cumblingObj__

#include "itemObj.h"
#include <iostream>

class CrumblingObj : public ItemObj {
private:
    typedef ItemObj super;
  
    void tick();
    void collisionEffect(CubeObj*);
    
public:
    CrumblingObj();
};

#endif /* defined(__Cubior__cumblingObj__) */
