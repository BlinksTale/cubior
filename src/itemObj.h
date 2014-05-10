//
//  itemObj.h
//  Cubior
//
//  Created by Brian Handy on 5/10/14.
//
//

#ifndef __Cubior__itemObj__
#define __Cubior__itemObj__

#include <iostream>
#include "cubeObj.h"

class ItemObj : public CubeObj {
private:
    typedef CubeObj super;

protected:
    void collisionEffect(CubeObj*);
    
public:
    ItemObj();
    void init();
    virtual void setX(int);
    virtual void setY(int);
    virtual void setZ(int);
    virtual void setPos(int,int,int);
    
};

#endif /* defined(__Cubior__itemObj__) */
