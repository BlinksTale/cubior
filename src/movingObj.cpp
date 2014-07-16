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
  nonstickSurface = false; // stick vs nonstick for player moving with blocks
  permalocked = false;
  locked = true;
  timer = 0;
  direction = 1;
  buildup = 0;
}

void MovingObj::tick() {
  
  // Move isn't working right now but Change is.
//  this->moveX(1000.0f);
  // fixme: ChangeY will push you through the floor. D:
  // fixme: once outside some range of initial spawn, collision stops working
  // but it starts working again when you re-enter the initial zone
  // so it must have to do with how far we check for collision with each cube
  // (make sure moving objects update this value)
  int lastX = this->getX();
  if (buildup != 0 && buildup/direction < 0) {
    buildup += direction;
  } else {
    this->changeX(1*direction);
  }
  if (lastX == this->getX()) {
    // Haven't moved! Increase buildup in appropriate direction
    buildup += direction;
  }
//  this->changeY(1*(timer%6000>3000?-1:1));

  int timeSpan = 500;
  if (timer>timeSpan) {
    direction *= -1;
    timer -= timeSpan;
  }
  
  timer++;
  
  super::tick();
  
}

void MovingObj::collisionEffect(CubeObj* c) {
  
  super::collisionEffect(c);
}