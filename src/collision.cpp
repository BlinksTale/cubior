/*
 * collision.cpp
 * by Brian Handy
 * 2/29/12
 * Finds and applies all collisions
 */

#include "collision.h"
#include "cubeObj.h"
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
  int c1Width = c1->getWidth()/2;
  int c2Width = c2->getWidth()/2;

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

  // How much overlap there is total
  return collisionX1 - collisionX2;
}

void Collision::bounce(CubeObj* c1, CubeObj* c2) {

  // How much overlap there is total
  int diffX = getDiff(c1,c2,0);
  int diffY = getDiff(c1,c2,1);
  int diffZ = getDiff(c1,c2,2);

  cout << "diffX = " << diffX;
  cout << ", diffY = " << diffY;
  cout << ", diffZ = " << diffZ << "\n";

  bounceByDiff(c1,c2,diffX,diffY,diffZ);

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

  // Only change one dimension at a time, the lowest that isn't zero
  if (diffY != 0 && (!c1Grounded || !c2Grounded)
  && ((abs(diffY) < abs(diffX)) || diffX == 0) && ((abs(diffY) < abs(diffZ)) || diffZ == 0)) {
    cout << "Called changeY\n";
    if (!c1Locked) { c1->changeY(-diffY*c1Land/2); }
    if (!c2Locked) { c2->changeY( diffY*c2Land/2); }
    // then in case either one lands...
    if (diffY < 0) { c1->land(); }
    if (diffY > 0) { c2->land(); }
  } else if (diffZ != 0 && (abs(diffZ) < abs(diffX) || diffX == 0)) {
    cout << "Called changeZ\n";
    if (!c1Locked) { c1->changeZ(-diffZ*c1Bounce/2); }
    if (!c2Locked) { c2->changeZ( diffZ*c2Bounce/2); }
  } else if (diffX != 0) {
    cout << "Called changeX\n";
    if (!c1Locked) { c1->changeX(-diffX*c1Bounce/2); }
    if (!c2Locked) { c2->changeX( diffX*c2Bounce/2); }
  } else {
    cout << "Called nothing\n";
  }

  cout << "diffY = " << diffY << ", c1Bounce = " << c1Bounce << ", c2Bounce = " << c2Bounce << "\n";
  cout << "c1.y = " << c1->getY() << ", c2.y = " << c2->getY() << "\n";
}

void Collision::bouncePrecisely(CubeObj* c1, CubeObj* c2) {
  int bounceDivisor = 1;
  int diffX = (c1->getX()-c2->getX());
  int diffY = (c1->getY()-c2->getY());
  int diffZ = (c1->getZ()-c2->getZ());

  if (diffX > 0) { diffX -= c1->getWidth()/2 + c2->getWidth()/2; }
  else { diffX += c1->getWidth()/2 + c2->getWidth()/2; }
  if (diffY > 0) { diffY -= c1->getWidth()/2 + c2->getWidth()/2; }
  else { diffY += c1->getWidth()/2 + c2->getWidth()/2; }
  if (diffZ > 0) { diffZ -= c1->getWidth()/2 + c2->getWidth()/2; }
  else { diffZ += c1->getWidth()/2 + c2->getWidth()/2; }
  
  c1->changeX(-1);
  c1->changeY(-1);
  c1->changeZ(-1);
  c2->changeX(1);
  c2->changeY(1);
  c2->changeZ(1);
}


void Collision::balanceMomentum(CubeObj* c1, CubeObj* c2) {
  int newX = (c1->getMomentumX() + c2->getMomentumX())/2;
  int newY = (c1->getMomentumY() + c2->getMomentumX())/2;
  int newZ = (c1->getMomentumZ() + c2->getMomentumX())/2;
  c1->setMomentumX(newX);
  c1->setMomentumY(newY);
  c1->setMomentumZ(newZ);
  c2->setMomentumX(newX);
  c2->setMomentumY(newY);
  c2->setMomentumZ(newZ);
}
