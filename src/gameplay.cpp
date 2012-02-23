/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#include <iostream>
#include <cstdlib>
#include "gameplay.h"
#include "cubeObj.h"
#include "cubiorObj.h"

using namespace std;

CubiorObj cubior[4];
CubeObj cube[4];
static int movementSpeed = 1;
static int jumpSpeedRatio = 5;
static int rotationSpeed = 10;
int maxJump = 25;
int maxSpeed = 20;
int friction = 1;

int gravity = 2;
int floor = -100;

void gameplayStart() {
  cubior[0].setPos(0,0,-1000);
  cubior[0].moveZ(3);
  cubior[1].setPos(-400,0,-1000);
  cubior[1].moveX(2);
  cubior[1].moveZ(3);
  cube[0].setPos(-200,0,-1300);
  cube[0].moveX(2);
  cube[0].moveZ(3);
  cube[0].setLock(true);

  cubior[0].setHappiness(1.0);
  cubior[1].setHappiness(0.5);
}

void gameplayLoop() {
  cubior[0].tick();
  cubior[1].tick();
  cube[0].tick();
  if (collision(&cubior[0],&cubior[1])) {
    bounce(&cubior[0],&cubior[1]);
    balanceMomentum(&cubior[0],&cubior[1]);
  }
  for (int i = 0; i<4; i++) {
    if (collision(&cubior[i],&cube[0])) {
      bounce(&cubior[i],&cube[0]);
      balanceMomentum(&cubior[i],&cube[0]);
    }
  }
}

bool collision(CubeObj* c1, CubeObj* c2) {
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

void bounce(CubeObj* c1, CubeObj* c2) {

  // Grabbing these vars only once from cubes, used many times here  
  int c1x = c1->getX();
  int c2x = c2->getX();
  int c1y = c1->getY();
  int c2y = c2->getY();
  int c1z = c1->getZ();
  int c2z = c2->getZ();

  // Width/2, the radius essentially
  int c1rad = c1->getWidth()/2;
  int c2rad = c2->getWidth()/2;
  int c1hgt = c1->getHeight()/2;
  int c2hgt = c2->getHeight()/2;

  // Collision points: where exactly the boxes are colliding
  int collisionX1 = c1x < c2x ? c1x + c1rad : c1x - c1rad;
  int collisionX2 = c2x < c1x ? c2x + c2rad : c2x - c2rad;
  int collisionY1 = c1y < c2y ? c1y + c1hgt : c1y - c1hgt;
  int collisionY2 = c2y < c1y ? c2y + c2hgt : c2y - c2hgt;
  int collisionZ1 = c1z < c2z ? c1z + c1rad : c1z - c1rad;
  int collisionZ2 = c2z < c1z ? c2z + c2rad : c2z - c2rad;

  // How much overlap there is total
  int diffX = collisionX1 - collisionX2;
  int diffY = collisionY1 - collisionY2;
  int diffZ = collisionZ1 - collisionZ2;

  // Only change one dimension at a time, the lowest that isn't zero
  if (diffY != 0 && (abs(diffY) < abs(diffX)) && (abs(diffY) < abs(diffZ))) {
    // getLock used so that locked objects don't bounce
    // and if other object is locked, you bounce double
    c1->changeY(-diffY*(1-c1->getLock()*1+c2->getLock()*1)/2);
    c2->changeY( diffY*(1-c2->getLock()*1+c1->getLock()*1)/2);
    // then in case you land...
    if (diffY < 0) { c1->land(); }
    if (diffY > 0) { c2->land(); }
  } else if (diffZ != 0 && abs(diffZ) < abs(diffX)) {
    c1->changeZ(-diffZ*(1-c1->getLock()*1+c2->getLock()*1)/2);
    c2->changeZ( diffZ*(1-c2->getLock()*1+c1->getLock()*1)/2);
  } else if (diffX != 0) {
    c1->changeX(-diffX*(1-c1->getLock()*1+c2->getLock()*1)/2);
    c2->changeX( diffX*(1-c2->getLock()*1+c1->getLock()*1)/2);
  }
}

void bouncePrecisely(CubeObj* c1, CubeObj* c2) {
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

void balanceMomentum(CubeObj* c1, CubeObj* c2) {
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

// Returns gameplay state
CubiorObj* getPlayer() { return &cubior[0]; }
CubiorObj* getPlayer(int i) { return &cubior[i]; }
CubeObj* getCube() { return &cube[0]; }
CubeObj* getCube(int i) { return &cube[i]; }

bool getInvincibility(int n) { return cubior[n].getInvincibility(); }
void setInvincibility(int n, bool newState) { cubior[n].setInvincibility(newState); }

int getFloor() { return floor; }
int getGravity() { return gravity; }
