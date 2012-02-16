/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#include <iostream>
#include "gameplay.h"
#include "cubeObj.h"
#include "cubiorObj.h"

using namespace std;

CubiorObj cubior[4];
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
  cubior[0].setHappiness(1.0);
  cubior[1].setHappiness(0.5);
}

void gameplayLoop() {
  cubior[0].tick();
  cubior[1].tick();
  if (collision(&cubior[0],&cubior[1])) {
    //bounce(&cubior[0],&cubior[1]);
    balanceMomentum(&cubior[0],&cubior[1]);
  }
}

bool collision(CubiorObj* c1, CubiorObj* c2) {
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

void bounce(CubiorObj* c1, CubiorObj* c2) {
  c1->changeX((c1->getX()-c2->getX())/5);
  c1->changeY((c1->getY()-c2->getY())/5);
  c1->changeZ((c1->getZ()-c2->getZ())/5);
  c2->changeX((c2->getX()-c1->getX())/5);
  c2->changeY((c2->getY()-c1->getY())/5);
  c2->changeZ((c2->getZ()-c1->getZ())/5);
}

void balanceMomentum(CubiorObj* c1, CubiorObj* c2) {
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

bool getInvincibility(int n) { return cubior[n].getInvincibility(); }
void setInvincibility(int n, bool newState) { cubior[n].setInvincibility(newState); }

int getFloor() { return floor; }
int getGravity() { return gravity; }
