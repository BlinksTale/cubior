/*
 * cubeObj.cpp
 * by Brian Handy
 * 2/11/12
 * Represents any cube in the game (obstacles, items and players)
 */

#include "cubeObj.h"
#include "gameplay.h"

#include <iostream>
#include <string> // for material
#include <cmath> // for atan
using namespace std;

CubeObj::CubeObj() {
  
  // material default
  material = "grass";

  // Pos vars
  x =    0;
  y = -200;
  z = 1000;
  oldX = 0;
  oldY = 0;
  oldZ = 0;
  diffX = 0;
  diffY = 0;
  diffZ = 0;

  // Movement vars
  movementSpeed =     1;
  movementDivision = 10;
  momentumX = 0 ;
  momentumY = 0 ;
  momentumZ = 0 ;

  maxSpeed = 20 ;
  friction = 1  ;

  // Jumping vars
  jumpable = false;
  grounded = false;
  lastGrounded = false;
  maxJump = 25 ;
  jumpSpeedRatio = 5 ;

  // Locking vars
  locked = false;
  lockable = true;
  permalocked = false;
  loseMomentumOnLock = false;

  // Collision vars
  collision = false;
  
  // World vars
  gravity = getGravity();
}

// Set the material, for coloring
void CubeObj::setMaterial(string s) {
  material = s;
}

void CubeObj::tick() {
  // don't move if frozen
  if (!locked && !permalocked) {

    if (collision) { setCollision(false); }

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
    lastGrounded = grounded;
    
    calculateDiff();

  // Can lose all momentum on locking if bool is set
  } else if (moving() && loseMomentumOnLock) { freeze(); }

  // Momentum loss & gravity apply if free or locked-and-loseMomentum-on-Lock
  if ((!locked || loseMomentumOnLock) && !permalocked) { fall(); }
}

void CubeObj::calculateDiff() {
    // And set diff vals last
    if (abs(1.0*x-oldX) > 2) { diffX = x - oldX; }
    if (abs(1.0*y-oldY) > 2) { diffY = y - oldY; }
    if (abs(1.0*z-oldZ) > 2) { diffZ = z - oldZ; }
    // Then update old vals
    oldX = x;
    oldY = y;
    oldZ = z;
}
// Apply gravity!
void CubeObj::fall() {
  momentumY -= gravity;
  grounded = false;
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
bool CubeObj::getStillGrounded() { return lastGrounded && grounded; }
bool CubeObj::getNotGrounded() { return !lastGrounded && !grounded; }
bool CubeObj::getLanded() { return grounded; }

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
int CubeObj::get(int s) { return s == 0 ? x : s == 1 ? y : z; }
int CubeObj::getX() { return x; }
int CubeObj::getY() { return y; }
int CubeObj::getZ() { return z; }
int CubeObj::getAngleX() { return 0; }
int CubeObj::getAngleY() { return atan(diffX*1.0/(diffZ != 0 ? diffZ : 1))*60+180*(diffZ>0); }
int CubeObj::getAngleZ() { return 0; }
int CubeObj::getMomentumX() { return momentumX * movementDivision; }
int CubeObj::getMomentumY() { return momentumY * movementDivision; }
int CubeObj::getMomentumZ() { return momentumZ * movementDivision; }

void CubeObj::setNeighbors(bool x1, bool x2, bool y1, bool y2, bool z1, bool z2) {
  neighbors[0] = x1;
  neighbors[1] = x2;
  neighbors[2] = y1;
  neighbors[3] = y2;
  neighbors[4] = z1;
  neighbors[5] = z2;
}
bool* CubeObj::getNeighbors() { return neighbors; }

// For being up against an edge of the map - used mostly in collision detection
void CubeObj::setEdges(bool x1, bool x2, bool y1, bool y2, bool z1, bool z2) {
  edges[0] = x1;
  edges[1] = x2;
  edges[2] = y1;
  edges[3] = y2;
  edges[4] = z1;
  edges[5] = z2;
}
bool* CubeObj::getEdges() { return edges; }
void CubeObj::setCollision(bool b) { collision = b; }
bool CubeObj::getCollision() { return collision; }

int CubeObj::getWidth() { return 100*(1); }
int CubeObj::getHeight() { return 100*(1); }
int CubeObj::getSize(int s) { return s == 1 ? 100 : 100; } // was getHeight and getWidth but Segfaulted on Mac. Read: I need to learn virtual functions better
string CubeObj::getMaterial() { return material; }
