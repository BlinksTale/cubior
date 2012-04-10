/*
 * cubeObj.cpp
 * by Brian Handy
 * 2/11/12
 * Represents any cube in the game (obstacles, items and players)
 */

#include "cubeObj.h"
#include "gameplay.h"

#include <iostream>
using namespace std;

CubeObj::CubeObj() {
  precision = 2;

  // Pos vars
  x =    0*precision;
  y = -200*precision;
  z = 1000*precision;

  // Movement vars
  movementSpeed =     1*precision;
  movementDivision = 10*precision;
  momentumX = 0 *precision;
  momentumY = 0 *precision;
  momentumZ = 0 *precision;

  maxSpeed = 20 *precision;
  friction = 1  *precision;

  // Jumping vars
  jumpable = false;
  grounded = false;
  maxJump = 25 *precision;
  jumpSpeedRatio = 5 *precision;

  // Locking vars
  locked = false;
  lockable = true;
  permalocked = false;
  loseMomentumOnLock = false;

  // Collision vars
  collision = false;
  
  // World vars
  floor = getFloor();
  gravity = getGravity()*precision;
}

void CubeObj::tick() {
  // don't move if frozen
  if (!locked && !permalocked) {
    // cap momentum on ground
    if (momentumX > maxSpeed) { momentumX = maxSpeed; }
    if (momentumX < -maxSpeed) { momentumX = -maxSpeed; }
    if (momentumZ > maxSpeed) { momentumZ = maxSpeed; }
    if (momentumZ < -maxSpeed) { momentumZ = -maxSpeed; }
    // Then move by that momentum
    x += momentumX; 
    y += momentumY;
    z += momentumZ; 

    // apply friction if on the ground
    if (grounded) {
      if (momentumX > 0) { momentumX -= friction; }
      else if (momentumX < 0) { momentumX += friction; }
      else { momentumX = 0; }
      if (momentumZ > 0) { momentumZ -= friction; }
      else if (momentumZ < 0) { momentumZ += friction; }
      else { momentumZ = 0; }
    }
  // Can lose all momentum on locking if bool is set
  } else if (moving() && loseMomentumOnLock) { freeze(); }

  // Momentum loss & gravity apply if free or locked-and-loseMomentum-on-Lock
  if ((!locked || loseMomentumOnLock) && !permalocked) { fall(); }
}

// Apply gravity! Stop if you hit the floor
void CubeObj::fall() {
  if (y < floor) {
    y = floor; land();
  } else { momentumY -= gravity; grounded = false; } 
}

// Act as if you landed on ground
void CubeObj::land() {
  momentumY = 0; lockable = true; jumpable = true; grounded = true;
}

// Moving is any movement bool
bool CubeObj::moving() {
 return (momentumX != 0 || momentumZ != 0 || momentumY != 0);
}

// Jump is possible if you have hit the ground since last jump
void CubeObj::jump(bool n) {
  if (jumpable) {
    if (n && momentumY < maxJump) { moveY(jumpSpeedRatio*10/precision); } else { jumpable = false; }
  }
}

// Collision Effect defaults to nothing happening
void CubeObj::collisionEffect(CubeObj* c) { }

// Lock to stop midair
void CubeObj::setLock(bool n) {
  if ((lockable && n) || !n) { locked = n; }
}
bool CubeObj::getLock() { return locked; }

// Permalock ensures complete lack of movement
void CubeObj::setPermalock(bool n) { permalocked = n; }
bool CubeObj::getPermalock() { return permalocked; }

// and for Grounding
bool CubeObj::getGrounded() { return grounded; }

// Set is absolute positioning
void CubeObj::setX(int n) { x = n*precision; }
void CubeObj::setY(int n) { y = n*precision; }
void CubeObj::setZ(int n) { z = n*precision; }
void CubeObj::setPos(int n, int o, int p) { x = n*precision, y = o*precision, z = p*precision; }

// Change is relative positioning
void CubeObj::changeX(int n) { x += n*precision; }
void CubeObj::changeY(int n) { y += n*precision; }
void CubeObj::changeZ(int n) { z += n*precision; }
void CubeObj::changePos(int n, int o, int p) { x += n*precision; y += o*precision; z += p*precision; }

// SetMomentum is absolute momentum
void CubeObj::setMomentumX(int n) { momentumX = n*precision * movementSpeed / movementDivision; }
void CubeObj::setMomentumY(int n) { momentumY = n*precision * movementSpeed / movementDivision; }
void CubeObj::setMomentumZ(int n) { momentumZ = n*precision * movementSpeed / movementDivision; }

// Move is relative momentum
void CubeObj::moveX(int n) { momentumX += n*precision * movementSpeed / movementDivision; }
void CubeObj::moveY(int n) { momentumY += n*precision * movementSpeed / movementDivision; }
void CubeObj::moveZ(int n) { momentumZ += n*precision * movementSpeed / movementDivision; }
void CubeObj::movePos(int n, int o, int p) {
  momentumX += n*precision * movementSpeed / movementDivision;
  momentumY += o*precision * movementSpeed / movementDivision;
  momentumZ += p*precision * movementSpeed / movementDivision;
}

// Freeze stops momentum
void CubeObj::freeze() { momentumX = 0; momentumY = 0; momentumZ = 0; }

// Getters
int CubeObj::get(int s) { return s == 0 ? x/precision : s == 1 ? y/precision : z/precision; }
int CubeObj::getX() { return x/precision; }
int CubeObj::getY() { return y/precision; }
int CubeObj::getZ() { return z/precision; }
int CubeObj::getMomentumX() { return momentumX * movementDivision/precision; }
int CubeObj::getMomentumY() { return momentumY * movementDivision/precision; }
int CubeObj::getMomentumZ() { return momentumZ * movementDivision/precision; }

void CubeObj::setNeighbors(bool x1, bool x2, bool y1, bool y2, bool z1, bool z2) {
  neighbors[0] = x1;
  neighbors[1] = x2;
  neighbors[2] = y1;
  neighbors[3] = y2;
  neighbors[4] = z1;
  neighbors[5] = z2;
}
bool* CubeObj::getNeighbors() { return neighbors; }
void CubeObj::setCollision(bool b) { collision = b; }
bool CubeObj::getCollision() { return collision; }

int CubeObj::getPrecision() { return precision; }
int CubeObj::getSize(int s) { return s == 1 ? getHeight() : getWidth(); }
int CubeObj::getWidth() { return 100*(1); }
int CubeObj::getHeight() { return 100*(1); }
