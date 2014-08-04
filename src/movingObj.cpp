//
//  movingObj.cpp
//  Cubior
//
//  Created by Brian Handy on 7/13/14.
//
//

#include "movingObj.h"
#include "cubiorObj.h"
#include "gameplay.h"

bool usingBuildup = true;
bool usingMomentum = false;

MovingObj::MovingObj(Direction newDirection) {
  super::init();
  itemType = "moving";
  firstCollision = false;
  nonstickSurface = false; // stick vs nonstick for player moving with blocks
  permalocked = false;
//  locked = true;
  if (newDirection != West && newDirection != East) {
    lockedX = true;
  }
  if (newDirection != Up && newDirection != Down) {
    lockedY = true;
  }
  if (newDirection != North && newDirection != South) {
    lockedZ = true;
  }
  timer = 0;
  movingDirection = newDirection;
  buildupX = 0;
  buildupY = 0;
  buildupZ = 0;

  canBeLandedOn = true;

  slaveStatus = false;
  masterStatus = false;
}

void MovingObj::tick() {
  
  if (!slaveStatus) {
    if (!getGameplayFirstRunning() && getGameplayRunning()) {
      moveForwards(1);
    }
  
    // Commented out the super tick since it was giving us gravity
    // switched to if usingMomentum only
    if (usingMomentum) {
      super::tick();
    }
  } else {
    changeX(master->getX() - lastMasterX);
    changeY(master->getY() - lastMasterY);
    changeZ(master->getZ() - lastMasterZ);
    
    momentumX = master->getMomentumX();
    momentumY = master->getMomentumY();
    momentumZ = master->getMomentumZ();
    
    lastMasterX = master->getX();
    lastMasterY = master->getY();
    lastMasterZ = master->getZ();
  }
}

void MovingObj::collisionEffect(CubeObj* c) {
  if (c->isPlayer()) {
    // New approach: don't flip, just undo last motion and keep trying
    //flipDirection();
//    moveForwards(-1);
  }
  super::collisionEffect(c);
}

void MovingObj::moveForwards(int distance) {
  // Move isn't working right now but Change is.
  //  this->moveX(1000.0f);
  // fixme: ChangeY will push you through the floor. D:
  // fixme: once outside some range of initial spawn, collision stops working
  // but it starts working again when you re-enter the initial zone
  // so it must have to do with how far we check for collision with each cube
  // (make sure moving objects update this value)
  int lastX = this->getX();
  int lastY = this->getY();
  int lastZ = this->getZ();
  int movementX = distance*(movingDirection == East)  - distance*(movingDirection == West);
  int movementY = distance*(movingDirection == Up)    - distance*(movingDirection == Down);
  int movementZ = distance*(movingDirection == South) - distance*(movingDirection == North);
  
  if (buildupX != 0 && buildupX/movementX < 0) {
    if (usingBuildup) {
      buildupX += movementX;
    }
  } else {
    if (usingMomentum) {
      this->setMomentumX(movementX);
    } else {
      this->changeX(movementX);
    }
  }
  if (buildupY != 0 && buildupY/movementY < 0) {
    if (usingBuildup) {
      buildupY += movementY;
    }
  } else {
    if (usingMomentum) {
      this->setMomentumY(movementY);
    } else {
      this->changeY(movementY);
    }
  }
  if (buildupZ != 0 && buildupZ/movementZ < 0) {
    if (usingBuildup) {
      buildupZ += movementZ;
    }
  } else {
    if (usingMomentum) {
      this->setMomentumZ(movementZ);
    } else {
      this->changeZ(movementZ);
    }
  }
  
  // Haven't moved! Increase buildup in appropriate direction
  if (usingBuildup) {
    if (lastX == this->getX()) {
      buildupX += movementX;
    }
    if (lastY == this->getY()) {
      buildupY += movementY;
    }
    if (lastZ == this->getZ()) {
      buildupZ += movementZ;
    }
  }
  
  
  timer += distance;
  
  int timeSpan = 500;
  if (timer>timeSpan) {
    flipDirection();
  }

}

void MovingObj::flipDirection() {
  switch (movingDirection)
  {
    case East:
      movingDirection = West;
      break;
    case West:
      movingDirection = East;
      break;
    case North:
      movingDirection = South;
      break;
    case South:
      movingDirection = North;
      break;
    case Up:
      movingDirection = Down;
      break;
    case Down:
      movingDirection = Up;
      break;
  }

  timer = 0;

}

void MovingObj::postNeighborInit() {
  
  // Slave status check requires neighbors to be known
  this->checkSlaveStatus();
  
  super::postNeighborInit();
}

void MovingObj::checkSlaveStatus() {
  int i=0;
  int neighborCount = 0;
  int movingNeighborCount = 0;
  while (i<6 && slaveStatus == false) {
//    cout << "Any neighbors for " << i << "? " << this->neighbors[i] << endl;
    if (this->neighbors[i]) {
      neighborCount++;
//      cout << "Yes with type " << this->visibleNeighborObjects[i]->getType() << endl;
      
      // Then process its slave/master status
      MovingObj* neighbor = (MovingObj*)this->visibleNeighborObjects[i];
      if (neighbor->getType().compare("moving") == 0 && neighbor->getMovingDirection() == movingDirection) {
        movingNeighborCount++;
        if (neighbor->isSlave()) {
          // both moving and a slave? We must be a slave too then!
          this->setSlave(true);
          this->setMaster(neighbor->getMaster());
        }
        if (neighbor->isMaster()) {
          this->setSlave(true);
          this->setMaster(neighbor);
        }
      }
    }
    i++;
  }
  
  // Still not a slave? Must be master!
  if (!slaveStatus) {
    this->setMaster(true);
  }
  // Either way, now spread this master to all your neighbors
  this->spreadMaster();
}

void MovingObj::spreadMaster() {
  // Check all neighbors again. If any not slaves and not master, set their master to ours
  // and call this function on them too
  for (int i=0; i<6; i++) {
    if (this->neighbors[i]) {
      if (this->visibleNeighborObjects[i]->getType().compare("moving") == 0) {
        MovingObj* neighbor = (MovingObj*)(this->visibleNeighborObjects[i]);
        if (!neighbor->isSlave() && !neighbor->isMaster() && neighbor->getMovingDirection() == movingDirection) {
          neighbor->setMaster(this->getMaster());
          neighbor->setSlave(true);
          neighbor->spreadMaster();
          // Alternative to the above three lines:
          //neighbor->checkSlaveStatus();
          // but commented out for now since those may work faster
        }
      }
    }
  }
}

bool MovingObj::isMaster() {
  return masterStatus;
}
void MovingObj::setMaster(bool b) {
  masterStatus = b;
  if (b) {
    master = this;
  }
}
bool MovingObj::isSlave() {
  return slaveStatus;
}
void MovingObj::setSlave(bool b) {
  slaveStatus = b;
}

void MovingObj::setMaster(CubeObj* incoming) {
  if (incoming != NULL && incoming->getType().compare("moving") == 0) {
    master = incoming;
    lastMasterX = master->getX();
    lastMasterY = master->getY();
    lastMasterZ = master->getZ();
  }
}

CubeObj* MovingObj::getMaster() {
  return masterStatus ? this : master;
}

MovingObj::Direction MovingObj::getMovingDirection() {
  return movingDirection;
}