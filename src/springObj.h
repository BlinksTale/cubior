//
//  springObj.h
//  Cubior
//
//  Created by Brian Handy on 4/8/14.
//
//

#ifndef __Cubior__springObj__
#define __Cubior__springObj__

#include "itemObj.h"
#include <iostream>


class SpringObj : public ItemObj {
private:
    typedef ItemObj super;

    void collisionEffect(CubeObj*);
    
public:
    SpringObj();
    
    void setX(int);
    void setY(int);
    void setZ(int);
    void setPos(int,int,int);

};

#endif /* defined(__Cubior__springObj__) */
