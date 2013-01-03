/*
 * collision.cpp
 * by Brian Handy
 * 2/29/12
 * Finds and applies all collisions
 */

#include "collision.h"
#include "cubeObj.h"
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
  // Whatever special effects happen, call them post-bounce
  c1->collisionEffect(c2);
  c2->collisionEffect(c1);
}

void Collision::bounceByDiff(CubeObj* c1, CubeObj* c2, int diffX, int diffY, int diffZ) {
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
  if (
      diffY != 0 &&// ((diffY > 0 && !c1e[2] && !c2e[3]) || (diffY < 0 && !c2e[2] && !c1e[3])) &&
    // Normal case: just make sure you aren't comparing in a dimension with no difference
    ((
        ((abs(diffY) < abs(diffX)) || diffX == 0)
      &&
        ((abs(diffY) < abs(diffZ)) || diffZ == 0)
    // Crazy case: other block is an edge block with neighbors
    /*) || ( // Even if you're not the smallest
        ( // If it's going to go off an edge and you're the second smallest
          (c1e[1] && diffX < 0 && abs(diffX) <= abs(diffZ) && abs(diffY) <= abs(diffZ))
        ||
          (c2e[1] && diffX > 0 && abs(diffX) <= abs(diffZ) && abs(diffY) <= abs(diffZ))
        ||
          (c1e[5] && diffZ < 0 && abs(diffZ) <= abs(diffX) && abs(diffY) <= abs(diffX))
        ||
          (c2e[5] && diffZ > 0 && abs(diffZ) <= abs(diffX) && abs(diffY) <= abs(diffX))
        ) // c1 has X or Z edges
    //  ) || (
    //    (c2e[0] || c2e[1] || c2e[4] || c2e[5]) // c2 has X or Z edges
    ) || ( // last alternative, x and z have neighbors/edges, so we must use Y
      (!c1Locked && (c2e[0] || c2n[0]) && (c2e[1] || c2n[1]) && (c2e[4] || c2n[4]) && (c2e[5] || c2n[5]) && abs(diffY) <= abs(diffX) && abs(diffY) <= abs(diffZ))
      ||
      (!c2Locked && (c1e[0] || c1n[0]) && (c1e[1] || c1n[1]) && (c1e[4] || c1n[4]) && (c1e[5] || c1n[5]) && abs(diffY) <= abs(diffX) && abs(diffY) <= abs(diffZ))
    */))
  ) {
    //cout << "CHOSE y DIFF " << diffY << endl;
    if (!c1Locked) { c1->changeY(-diffY*c1Land/2); }
    if (!c2Locked) { c2->changeY( diffY*c2Land/2); }
    // balanceMomentum(c1,c2,1);
    // then in case either one lands...
    if (diffY < 0) {
      c1->land();
      if (c2->isPlayer()) {
        c1->landOn(c2);
      }
      if (!c1Locked) { c1->changeY(1); } // extra 1 of movement since landing caused sticking before
    }
    if (diffY > 0) {
      c2->land();
      if (!c2Locked) { c2->changeY(1); }
      if (c1->isPlayer()) {
        c2->landOn(c1);
      }
    }
  } else if
    (
        diffZ != 0
      && //((diffZ > 0 && !c1e[4] && !c2e[5]) || (diffZ < 0 && !c2e[4] && !c1e[5])) && 
      (
        (
          abs(diffZ) < abs(diffX) || diffX == 0
        /*) || ( // Even if you're not the smallest
          ( // If it's going to go off an edge and you're the second smallest
            // choose that route instead!
            (c1e[1] && diffX < 0)
          ||
            (c2e[1] && diffX > 0)
          ||
            (c2e[0] && diffX > 0)
          ||
            (c2e[0] && diffX > 0)
          )*/
        )
      )
    ) {
    //cout << "CHOSE z DIFF " << diffZ << endl;
    if (!c1Locked) { c1->changeZ(-diffZ*c1Bounce/2); }
    if (!c2Locked) { c2->changeZ( diffZ*c2Bounce/2); }
  c1->setCollision(true);
  c2->setCollision(true);
  if (!c1->isPlayer() || !c2->isPlayer()) {
    c1->applyCollisionMomentumZ();
    c2->applyCollisionMomentumZ();
  }
//    balanceMomentum(c1,c2,2);
  } else if (diffX != 0) {// && ((diffX > 0 && !c1e[0] && !c2e[1]) || (diffX < 0 && !c2e[0] && !c1e[1]))) {
    //cout << "CHOSE x DIFF " << diffX << endl;
    if (!c1Locked) { c1->changeX(-diffX*c1Bounce/2); }
    if (!c2Locked) { c2->changeX( diffX*c2Bounce/2); }
  c1->setCollision(true);
  c2->setCollision(true);
  if (!c1->isPlayer() || !c2->isPlayer()) {
    c1->applyCollisionMomentumX();
    c2->applyCollisionMomentumX();
  }
  // balanceMomentum(c1,c2,0);
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
    }
	}
}
