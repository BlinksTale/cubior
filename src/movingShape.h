//
//  movingShape.h
//  Cubior
//
//  Created by Brian Handy on 7/13/14.
//
//

#ifndef __Cubior__movingShape__
#define __Cubior__movingShape__

#include <iostream>
#include "itemShape.h"
#include "cubeObj.h"

class MovingShape : public ItemShape {
private:
  typedef ItemShape super;
protected:
public:
  void initVisuals();
  void draw();
};


#endif /* defined(__Cubior__movingShape__) */
