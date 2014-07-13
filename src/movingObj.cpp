//
//  movingObj.cpp
//  Cubior
//
//  Created by Brian Handy on 7/13/14.
//
//

#include "movingObj.h"
#include "cubiorObj.h"

MovingObj::MovingObj() {
  super::init();
  itemType = "moving";
  firstCollision = false;
  
}

void MovingObj::tick() {
  
  // Move isn't working right now but Change is.
//  this->moveX(1000.0f);
  this->changeX(1);
  
  super::tick();
  
}

void MovingObj::collisionEffect(CubeObj* c) {
  
  super::collisionEffect(c);
}