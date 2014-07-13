//
//  movingObj.cpp
//  Cubior
//
//  Created by Brian Handy on 7/13/14.
//
//

#include "movingObj.h"
#include "cubiorObj.h"

int timer = 0;

MovingObj::MovingObj() {
  super::init();
  itemType = "moving";
  firstCollision = false;
  nonstickSurface = false; // stick vs nonstick for player moving with blocks
  permalocked = false;
}

void MovingObj::tick() {
  
  // Move isn't working right now but Change is.
//  this->moveX(1000.0f);
  // fixme: ChangeY will push you through the floor. D:
  // fixme: once outside some range of initial spawn, collision stops working
  // but it starts working again when you re-enter the initial zone
  // so it must have to do with how far we check for collision with each cube
  // (make sure moving objects update this value)
  this->changeX(1*(timer%10000>5000?-1:1));
//  this->changeY(1*(timer%6000>3000?-1:1));
  timer++;
  
  super::tick();
  
}

void MovingObj::collisionEffect(CubeObj* c) {
  
  super::collisionEffect(c);
}