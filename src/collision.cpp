/*
 * collision.cpp
 * by Brian Handy
 * 2/29/12
 * Finds and applies all collisions
 */

#include "collision.h"
#include "cubeObj.h"
#include "movingObj.h"
#include "gameplay.h" // for map width/height/depth stuff
#include <iostream>
#include <cstdlib>

using namespace std;

Collision::Collision() {
    int totalCollisions = 0; 
}

bool Collision::between(CubeObj* c1, CubeObj* c2) {
  int c1x = c1->getX();
  int c1y = c1->getY();
  int c1z = c1->getZ();
  int c2x = c2->getX();
  int c2y = c2->getY();
  int c2z = c2->getZ();
  // Very oddly, just getWidth causes segfaults on Macs at level transitions. getDiff's getSize seems to work fine
	int c1Width = 50;//c1->getWidth()/2;
	int c2Width = 50;//c2->getWidth()/2;

  return
     (c1x + c1Width > c2x - c2Width) &&
     (c1x - c1Width < c2x + c2Width) &&
     (c1y + c1Width > c2y - c2Width) &&
     (c1y - c1Width < c2y + c2Width) &&
     (c1z + c1Width > c2z - c2Width) &&
     (c1z - c1Width < c2z + c2Width);
}

int Collision::getDiff(CubeObj* c1, CubeObj* c2, int dimension) {
  // Grabbing these vars only once from cubes, used many times here  
  int c1x = c1->get(dimension);
  int c2x = c2->get(dimension);

  // Width/2, the radius essentially
  int c1rad = c1->getSize(dimension)/2;
  int c2rad = c2->getSize(dimension)/2;

  // Collision points: where exactly the boxes are colliding
  int collisionX1 = c1x < c2x ? c1x + c1rad : c1x - c1rad;
  int collisionX2 = c2x < c1x ? c2x + c2rad : c2x - c2rad;

  int result = collisionX1 - collisionX2;

  // How much overlap there is total
  return result;
}

void Collision::bounce(CubeObj* c1, CubeObj* c2) {

  // How much overlap there is total
  int diffX = getDiff(c1,c2,0);
  int diffY = getDiff(c1,c2,1);
  int diffZ = getDiff(c1,c2,2);
  bounceByDiff(c1,c2,diffX,diffY,diffZ);
}

void Collision::bounceByDiff(CubeObj* c1, CubeObj* c2, int diffX, int diffY, int diffZ) {
  // If moving type, swap slave with master for effects
  // we do this earlier now (before we did it after checking locked/grounded/etc)
  // since the old way allowed for odd changes to Y moving obj positions
  if (c1->getType().compare("moving") == 0) {
    if (((MovingObj*)c1)->isSlave()) {
      c1 = ((MovingObj*)c1)->getMaster();
    }
  }
  if (c2->getType().compare("moving") == 0) {
    if (((MovingObj*)c2)->isSlave()) {
      c2 = ((MovingObj*)c2)->getMaster();
    }
  }
  
  // getLock used so that locked objects don't bounce
  // and if other object is locked, you bounce double
  bool c1Locked = (c1->getLock()||c1->getPermalock());
  bool c2Locked = (c2->getLock()||c2->getPermalock());
  bool c1Grounded = c1->getGrounded();
  bool c2Grounded = c2->getGrounded();
  int c1Bounce = (1-c1Locked*1+c2Locked*1);
  int c2Bounce = (1-c2Locked*1+c1Locked*1);
  int c1Land = (1-(c1Locked||c1Grounded)*1+(c2Locked||c2Grounded)*1);
  int c2Land = (1-(c2Locked||c2Grounded)*1+(c1Locked||c1Grounded)*1);

  bool* c1e = c1->getEdges();
  bool* c1n = c1->getNeighbors();
  bool* c2e = c2->getEdges();
  bool* c2n = c2->getNeighbors();

  //if (diffX != 0 || diffY != 0 || diffZ != 0) {
    //cout << "DIFFS FOR " << c1 << " AND " << c2 << " ARE " << diffX << ", " << diffY << ", " << diffZ << endl;
  //}

  // Only change one dimension at a time, the lowest that isn't zero
  // Normal case: just make sure you aren't comparing in a dimension with no difference
  // Is Y the direction of least resistance?
  if (diffY != 0 && (( ((abs(diffY) < abs(diffX)) || diffX == 0) && ((abs(diffY) < abs(diffZ)) || diffZ == 0)))) {
    //cout << "CHOSE y DIFF " << diffY << endl;
    if (!c1Locked && !c1->getLockY()) { c1->changeY(-diffY*c1Land/2); }
    if (!c2Locked && !c2->getLockY()) { c2->changeY( diffY*c2Land/2); }
    // balanceMomentum(c1,c2,1);
    // then in case either one lands...
    if (diffY < 0) {
      c1->land();
      if (c2->isLandable()) {
        c1->landOn(c2);
      }
      if (!c1Locked && !c1->getLockY()) { c1->changeY(1); } // extra 1 of movement since landing caused sticking before
    }
    if (diffY > 0) {
      c2->land();
      if (!c2Locked && !c2->getLockY()) { c2->changeY(1); }
      if (c1->isLandable()) {
        c2->landOn(c1);
      }
    }
  // Is Z the direction of least resistance?
  } else if (diffZ != 0 && ((abs(diffZ) < abs(diffX) || diffX == 0))) {
    //cout << "CHOSE z DIFF " << diffZ << endl;
    if (!c1Locked && !c1->getLockZ()) { c1->changeZ(-diffZ*c1Bounce/2); }
    if (!c2Locked && !c2->getLockZ()) { c2->changeZ( diffZ*c2Bounce/2); }
    c1->setCollision(true);
    c2->setCollision(true);
    if (!c1->isPlayer() || !c2->isPlayer()) {
      c1->applyCollisionMomentumZ();
      c2->applyCollisionMomentumZ();
    }
    //balanceMomentum(c1,c2,2);
  // Is X the direction of least resistance?
  } else if (diffX != 0) {
    //cout << "CHOSE x DIFF " << diffX << endl;
    if (!c1Locked && !c1->getLockX()) { c1->changeX(-diffX*c1Bounce/2); }
    if (!c2Locked && !c2->getLockX()) { c2->changeX( diffX*c2Bounce/2); }
    c1->setCollision(true);
    c2->setCollision(true);
    if (!c1->isPlayer() || !c2->isPlayer()) {
      c1->applyCollisionMomentumX();
      c2->applyCollisionMomentumX();
    }
    //balanceMomentum(c1,c2,0);
  }
}

void Collision::bouncePrecisely(CubeObj* c1, CubeObj* c2) {
  int bounceDivisor = 1;
  int diffX = (c1->getX()-c2->getX());
  int diffY = (c1->getY()-c2->getY());
  int diffZ = (c1->getZ()-c2->getZ());
  int c1Width = 100; // c1->getWidth() // Real value disabled since broken on Macs for new levels
  int c2Width = 100; // c2->getWidth() // Real value disabled since broken on Macs for new levels
	
  if (diffX > 0) { diffX -= c1Width/2 + c2Width/2; }
  else { diffX += c1Width/2 + c2Width/2; }
  if (diffY > 0) { diffY -= c1Width/2 + c2Width/2; }
  else { diffY += c1Width/2 + c2Width/2; }
  if (diffZ > 0) { diffZ -= c1Width/2 + c2Width/2; }
  else { diffZ += c1Width/2 + c2Width/2; }
  
  if (c1->getType().compare("moving") == 0 && ((MovingObj*)c1)->isSlave()) {
    c1 = ((MovingObj*)c1)->getMaster();
  }
  if (c2->getType().compare("moving") == 0 && ((MovingObj*)c2)->isSlave()) {
    c2 = ((MovingObj*)c2)->getMaster();
  }
  c1->changeX(-1);
  c1->changeY(-1);
  c1->changeZ(-1);
  c2->changeX(1);
  c2->changeY(1);
  c2->changeZ(1);
}


void Collision::balanceMomentum(CubeObj* c1, CubeObj* c2, int deg) {
  if (deg == 0) {
    int mX1 = c1->getMomentumX();
    int mX2 = c2->getMomentumX();
    c1->setMomentumX((mX2*1+mX1*1)/2);
    c2->setMomentumX((mX1*1+mX2*1)/2);
  } else if (deg == 1) {
    int mY1 = c1->getMomentumY();
    int mY2 = c2->getMomentumY();
    c1->setMomentumY((mY2*1+mY1*1)/2);
    c2->setMomentumY((mY1*1+mY2*1)/2);
  } else {
    int mZ1 = c1->getMomentumZ();
    int mZ2 = c2->getMomentumZ();
    c1->setMomentumZ((mZ2*1+mZ1*1)/2);
    c2->setMomentumZ((mZ1*1+mZ2*1)/2);
  }
}

void Collision::checkAndBounce(CubeObj* c1, CubeObj* c2) {
  if (c1 != NULL && c2 != NULL) {
    // Now make sure it's not a cam checking against invisible things
    if ((!c1->isCamera() || !c2->isInvisible()) && (!c2->isCamera() || !c1->isInvisible())) {
      //if (!(c2->isInvisible()) && !(c1->isInvisible())) {
      if (between(c1,c2)) {
        // No player commands if the camera hits something
        if (c1->isCamera() && !(c2->isInvisible())) {
          ((CameraObj*)c1)->hitSomething();
        }
        if (c2->isCamera() && !(c1->isInvisible())) {
          ((CameraObj*)c2)->hitSomething();
        }
        
        // Separate effects from physically pushing against each other
        if (c1->isSolid() && c2->isSolid()) {
          // Then bounce the objects off each other
          bounce(c1,c2);
          // And if one above the other, land on that other
          if (c1->getLandedOnCount() > 0) {
            c1->updateLandedOnPos();
          }
          if (c2->getLandedOnCount() > 0) {
            c2->updateLandedOnPos();
          }
          //balanceMomentum(c1,c2);
        }
        
        // Whatever special effects happen, call them post-bounce
        
        if (c1->getType().compare("moving") == 0 && ((MovingObj*)c1)->isSlave()) {
          c1 = ((MovingObj*)c1)->getMaster();
        }
        if (c2->getType().compare("moving") == 0 && ((MovingObj*)c2)->isSlave()) {
          c2 = ((MovingObj*)c2)->getMaster();
        }
        c1->collisionEffect(c2);
        c2->collisionEffect(c1);
      }
      
    }
  }
}
