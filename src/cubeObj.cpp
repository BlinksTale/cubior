/*
 * cubeObj.cpp
 * by Brian Handy
 * 2/11/12
 * Represents any cube in the game (obstacles, items and players)
 */

#include "cubeObj.h"
#include "gameplay.h"

CubeObj::CubeObj() {
  x = 0;
  y = -200;
  z = 1000;

  movementSpeed = 1;
  locked = false;
  lockable = false;
  loseMomentumOnLock = false;
  jumpable = false;
  grounded = false;
  maxSpeed = 20;
  maxJump = 25;
  jumpSpeedRatio = 5;
  friction = 1;
  floor = getFloor();
  gravity = getGravity();
}

void CubeObj::tick() {
  // demo animation: angleZ += rotationSpeed / 10;
  // base smile on rotation
  //happiness = (angleZ % 360 - 120) / 360.0 * 2.0;
  // don't move if frozen
  if (!locked) {
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
  } else if (moving() && loseMomentumOnLock) { freeze(); }
  if (!locked || loseMomentumOnLock) { fall(); }
}

// Apply gravity! Stop if you hit the floor
void CubeObj::fall() {
  if (y < floor) {
    y = floor; momentumY = 0; lockable = true; jumpable = true; grounded = true;
  } else { momentumY -= gravity; grounded = false; } 
}

bool CubeObj::moving() {
 return (momentumX != 0 || momentumZ != 0 || momentumY != 0);
}

void CubeObj::jump(bool n) {
  if (jumpable) {
    if (n && momentumY < maxJump) { moveY(jumpSpeedRatio); } else { jumpable = false; }
  }
}

void CubeObj::setLock(bool n) {
  if ((lockable && n) || !n) { locked = n; }
}
bool CubeObj::getLock() { return locked; }

void CubeObj::setX(int n) { x = n; }
void CubeObj::setY(int n) { y = n; }
void CubeObj::setZ(int n) { z = n; }
void CubeObj::setPos(int n, int o, int p) { x = n, y = o, z = p; }

void CubeObj::changeX(int n) { x += n; }
void CubeObj::changeY(int n) { y += n; }
void CubeObj::changeZ(int n) { z += n; }
void CubeObj::changePos(int n, int o, int p) { x += n; y += o; z += p; }

void CubeObj::moveX(int n) { momentumX += n * movementSpeed; }
void CubeObj::moveY(int n) { momentumY += n * movementSpeed; }
void CubeObj::moveZ(int n) { momentumZ += n * movementSpeed; }
void CubeObj::movePos(int n, int o, int p) {
  momentumX += n * movementSpeed;
  momentumY += o * movementSpeed;
  momentumZ += p * movementSpeed;
}

void CubeObj::freeze() { momentumX = 0; momentumY = 0; momentumZ = 0; }

int CubeObj::getX() { return x; }
int CubeObj::getY() { return y; }
int CubeObj::getZ() { return z; }
