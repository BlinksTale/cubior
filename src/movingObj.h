//
//  movingObj.h
//  Cubior
//
//  Created by Brian Handy on 7/13/14.
//
//

#ifndef __Cubior__movingObj__
#define __Cubior__movingObj__

#include "itemObj.h"
#include <iostream>

class MovingObj : public ItemObj {
private:
  typedef ItemObj super;
  
  void tick();
  void collisionEffect(CubeObj*);
  
public:
  MovingObj();
};

#endif /* defined(__Cubior__movingObj__) */
