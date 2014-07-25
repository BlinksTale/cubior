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
public:
  static enum Direction { North, South, East, West, Up, Down };
  
private:
  typedef ItemObj super;
  
  bool masterStatus, slaveStatus;
  int lastMasterX, lastMasterY, lastMasterZ;
  
  CubeObj* master;
  
  int timer, buildupX, buildupY, buildupZ;
  Direction movingDirection;
  
  void tick();
  void moveForwards(int);
  void flipDirection();
  void collisionEffect(CubeObj*);

public:
  MovingObj(Direction);
  void postNeighborInit();
  void spreadMaster();
  void checkSlaveStatus();
  bool isSlave();
  void setSlave(bool);
  bool isMaster();
  void setMaster(bool);
  void setMaster(CubeObj*);
  CubeObj* getMaster();
  Direction getMovingDirection();
};

#endif /* defined(__Cubior__movingObj__) */
