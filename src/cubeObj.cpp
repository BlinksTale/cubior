/*
 * cubeObj.cpp
 * by Brian Handy
 * 2/11/12
 * Represents any cube in the game (obstacles, items and players)
 */

#include "cubeObj.h"
#include "gameplay.h"

CubeObj::CubeObj() {
  // Pos vars
  x = 0;
  y = -200;
  z = 1000;

  // Movement vars
  movementSpeed = 1;
  movementDivision = 10;
  momentumX = 0;
  momentumY = 0;
  momentumZ = 0;

  maxSpeed = 20;
  friction = 1;

  // Jumping vars
  jumpable = false;
  grounded = false;
  maxJump = 25;
  jumpSpeedRatio = 5;

  // Locking vars
  locked = false;
  lockable = true;
  permalocked = false;
  loseMomentumOnLock = false;

  // Collision vars
  collision = false;
  
  // World vars
  floor = getFloor();
  gravity = getGravity();
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
    if (n && momentumY < maxJump) { moveY(jumpSpeedRatio*10); } else { jumpable = false; }
  }
}

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
void CubeObj::setX(int n) { x = n; }
void CubeObj::setY(int n) { y = n; }
void CubeObj::setZ(int n) { z = n; }
void CubeObj::setPos(int n, int o, int p) { x = n, y = o, z = p; }

// Change is relative positioning
void CubeObj::changeX(int n) { x += n; }
void CubeObj::changeY(int n) { y += n; }
void CubeObj::changeZ(int n) { z += n; }
void CubeObj::changePos(int n, int o, int p) { x += n; y += o; z += p; }

// SetMomentum is absolute momentum
void CubeObj::setMomentumX(int n) { momentumX = n * movementSpeed / movementDivision; }
void CubeObj::setMomentumY(int n) { momentumY = n * movementSpeed / movementDivision; }
void CubeObj::setMomentumZ(int n) { momentumZ = n * movementSpeed / movementDivision; }

// Move is relative momentum
void CubeObj::moveX(int n) { momentumX += n * movementSpeed / movementDivision; }
void CubeObj::moveY(int n) { momentumY += n * movementSpeed / movementDivision; }
void CubeObj::moveZ(int n) { momentumZ += n * movementSpeed / movementDivision; }
void CubeObj::movePos(int n, int o, int p) {
  momentumX += n * movementSpeed / movementDivision;
  momentumY += o * movementSpeed / movementDivision;
  momentumZ += p * movementSpeed / movementDivision;
}

// Freeze stops momentum
void CubeObj::freeze() { momentumX = 0; momentumY = 0; momentumZ = 0; }

// Getters
int CubeObj::get(int a) { return a == 0 ? x : a == 1 ? y : z; }
int CubeObj::getX() { return x; }
int CubeObj::getY() { return y; }
int CubeObj::getZ() { return z; }
int CubeObj::getMomentumX() { return momentumX * movementDivision / movementSpeed; }
int CubeObj::getMomentumY() { return momentumY * movementDivision / movementSpeed; }
int CubeObj::getMomentumZ() { return momentumZ * movementDivision / movementSpeed; }

void CubeObj::setCollision(bool b) { collision = b; }
bool CubeObj::getCollision() { return collision; }

int CubeObj::getSize(int a) { return a == 0 || 2 ? getWidth() : getHeight(); }
int CubeObj::getWidth() { return 100*(1); }
int CubeObj::getHeight() { return 100*(1); }
