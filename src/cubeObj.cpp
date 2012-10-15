/*
 * cubeObj.cpp
 * by Brian Handy
 * 2/11/12
 * Represents any cube in the game (obstacles, items and players)
 */

#include "cubeObj.h"
#include "gameplay.h"

#include <iostream>
#include <cmath> // for atan
#define _USE_MATH_DEFINES // for M_PI
#include <math.h> // for M_PI
using namespace std;

CubeObj::CubeObj() {
  
  // Friction for all techniques
  newFriction = 2.35;
  friction =  2 ;
  direction = 0.0;
  strength = 0.0;

  // And movement limits & ratios
  maxSpeed = 25 ;
  movementSpeed = 0.3;

  // material default
  material = 0;
  
  // usually not a player
  playerStatus = false;
  toldToMove = false;

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
  movementDivision = 10;
  momentumX = 0 ;
  momentumY = 0 ;
  momentumZ = 0 ;
  
  // Jumping vars
  jumpable = false;
  grounded = false;
  lastGrounded = false;
  doubleLastGrounded = false;
  maxJump = 25 ;
  jumpSpeed = 1;
  jumpSpeedRatio = 5 ;

  // Locking vars
  locked = false;
  lockable = true;
  permalocked = false;
  loseMomentumOnLock = false;

  // Collision vars
  collision = false;
  landedOn = NULL;
  landedOnCount = 0;
  
  // World vars
  gravity = getGravity();
  neighborsSet = false;
}

void CubeObj::tick() {
  //cout << "Bgn Strength:  " << strength << " and Direction: " << direction << endl;
  //cout << "TICK CALLED";
  //if (toldToMove) { cout << " and TOLD TO MOVE" << endl; }
  //cout << endl;
  // don't move if frozen
  //cout << "OneB_01 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

  if (!locked && !permalocked) {

    if (collision) {
      setCollision(false);
    }
    //cout << "OneB_02 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

    // If on another player, move relative to where you were on them first
    // (since they may have moved, and brought you with them)
    if (landedOn != NULL) {
      x = landedOn->getX()+landedOnX;
      //y = landedOn->getY()+landedOnY;
      z = landedOn->getZ()+landedOnZ;
    }
    //cout << "OneB_03 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  
    
    // cap momentum on ground
    if (momentumX > maxSpeed) { momentumX = maxSpeed; }
    if (momentumX < -maxSpeed) { momentumX = -maxSpeed; }
    if (momentumZ > maxSpeed) { momentumZ = maxSpeed; }
    if (momentumZ < -maxSpeed) { momentumZ = -maxSpeed; }
    // Then move by that momentum
    x += momentumX; 
    y += momentumY;
    z += momentumZ;
    //cout << "OneB_03 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

    // apply friction if on the ground
  	if (!toldToMove) {
      if (grounded && (momentumX != 0 || momentumZ != 0)) {
        // OLD WAY
        /*if (momentumX > friction) { momentumX -= friction; }
        else if (momentumX <-friction) { momentumX += friction; }
        else if (momentumX != 0) { momentumX = 0; }
        if (momentumZ > friction) { momentumZ -= friction; }
        else if (momentumZ < -friction) { momentumZ += friction; }
        else if (momentumZ != 0) { momentumZ = 0; }*/
        // NEW WAY
        // change x/z into dir and str, then apply friction omnidirectionally/in proportion
        // and convert back to apply to momentum
        float dir = atan(momentumX*1.0/momentumZ);
        if (momentumZ < 0) { dir += M_PI; }
        float str = sqrt((float)(momentumX*momentumX) + (float)(momentumZ*momentumZ));
        if (str > newFriction) { str -= newFriction; }
        else if (str != 0.0) { str = 0.0; }
        // Now update direction... maybe, if str was strong enough
        if (str >= newFriction) {
          direction = dir;
        }
        // and always update strength
        if (strength != str) {
          strength = str;
        }
        // then lastly, transfer back to momentumX and Z
        momentumX = sin(direction)*strength;
        momentumZ = cos(direction)*strength;
      }
      //cout << "OneB_04a player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

	  } else {
	    toldToMove = false;
    
      // And since moving, update your direction!
      float dir = atan(momentumX*1.0/momentumZ);
      if (momentumZ < 0) { dir += M_PI; }
      float str = sqrt((float)(momentumX*momentumX) + (float)(momentumZ*momentumZ));
      // Now update direction... maybe, if str was strong enough
      if (str >= newFriction) {
        direction = dir;
      }
      // and always update strength
      if (strength != str) {
        strength = str;
      }
      //cout << "OneB_04b player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

	  }
    
    calculateDiff();

    //cout << "OneB_05 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

  // Can lose all momentum on locking if bool is set
  } else if (isMoving() && loseMomentumOnLock) { freeze(); }

  //cout << "OneB_06 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

  // Momentum loss & gravity apply if free or locked-and-loseMomentum-on-Lock
  if ((!locked || loseMomentumOnLock) && !permalocked) { fall(); }
  
  //cout << "OneB_07 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

  // And at the end, update your relation to the landedOn
  if (updateLandedOnPos()) {
    // Any changes? Also update your directional diff
    landedOnDirectionDiff = direction - landedOn->getDirection();
  }

  //cout << "OneB_08 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

  //cout << "End Strength:  " << strength << " and Direction: " << direction << endl;
}

float CubeObj::getStrength() {
  return strength;
}
float CubeObj::getDirection() {
  float result = direction;
  if (getLandedOnCount()>0) {
    result = landedOn->getDirection() + landedOnDirectionDiff;
  }
  return result;
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
  doubleLastGrounded = lastGrounded;
  lastGrounded = grounded;
  grounded = false;
  //cout << "fall lG is " << lastGrounded << " and g is " << grounded << endl;
  if (getNotGrounded() && landedOn != NULL) {
    momentumX += landedOn->getMomentumX();
    //momentumY += landedOn->getMomentumY();
    momentumZ += landedOn->getMomentumZ();
    direction = landedOn->getDirection() + landedOnDirectionDiff;
    landedOn = NULL;
    landedOnCount = 0;
    landedOnDirectionDiff = 0.0;
  } // not on a player anymore!
}

// Act as if you landed on ground
void CubeObj::land() {
  momentumY = 0;
  lockable = true;
  jumping = false;
  
  doubleLastGrounded = lastGrounded;
  lastGrounded = grounded;
  grounded = true;
  //cout << "land lG is " << lastGrounded << " and g is " << grounded << endl;
}

// to also land on another player
void CubeObj::landOn(CubeObj* c) {
  if (landedOn != c) {
    landedOn = c;
    landedOnCount = c->getLandedOnCount() + 1;
    landedOnDirectionDiff = direction - landedOn->getDirection();
    updateLandedOnPos();
  }
}

// Update relation between you and moving thing you landed on
bool CubeObj::updateLandedOnPos() {
  bool result = false;
  if (landedOn != NULL) {
    if (landedOnX != x-landedOn->getX()) {
      landedOnX = x-landedOn->getX();
      result = true;
    }
    if (landedOnY != y-landedOn->getY()) {
      landedOnY = y-landedOn->getY();
      result = true;
    }
    if (landedOnZ != z-landedOn->getZ()) {
      landedOnZ = z-landedOn->getZ();
      result = true;
    }
  }
  return result;
}

int CubeObj::getLandedOnCount() {
  return landedOnCount;
}

// isMoving is any movement bool
bool CubeObj::isMoving() {
 return (momentumX != 0 || momentumZ != 0 || momentumY != 0);
}

// isMoving is any movement bool
bool CubeObj::isMovingQuickly() {
 return !(momentumX < 10 && momentumZ < 10 && momentumX > -10 && momentumZ > -10);
}
// isPlayer returns whether just a normal block or a player
bool CubeObj::isPlayer() {
  return playerStatus;
}

// Jump is possible if you have hit the ground since last jump
void CubeObj::jump(bool n) {
  // Straight up, need to have landed and not tried to jump again before new jump
  //if (!n && this->getStillGrounded()) { newJump = true; }
  
  // If grounded, can jump again
  if (!n && getStillGrounded()) {
    jumpable = true;
  }
  
  //cout << "So jumpable is " << jumpable << " and newJump is " << newJump << " and getStillGrounded is " << this->getGrounded() << endl;
  // Try to jump!
  if (jumpable) {// || (getStillGrounded() && newJump)) {
    // To start a new jump off the ground
    //if (newJump) { newJump = false; jumpable = true; }
    // To keep an old jump flying
    if (n && momentumY < maxJump) {
      jumping = true; moveY(jumpSpeedRatio*10);
    } else if (!getStillGrounded()) {
      // if not grounded, can't jump again!
      jumpable = false;
    }
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
bool CubeObj::getStillGrounded() { return lastGrounded || grounded || doubleLastGrounded; }
bool CubeObj::getNotGrounded() { return !getStillGrounded(); }
bool CubeObj::getLanded() { return grounded; }
bool CubeObj::justJumped() {
  bool result = jumping && !lastJumping;
  lastJumping = jumping;
  return result;
}
bool CubeObj::justBumped() {
  bool result = collision && !lastCollision;
  lastCollision = collision;
  return result;
}

// Set is absolute positioning
void CubeObj::setX(int n) { x = n; }
void CubeObj::setY(int n) { y = n; }
void CubeObj::setZ(int n) { z = n; }
void CubeObj::setPos(int n, int o, int p) { x = n, y = o, z = p; }


// Find how close you are to another cube
int CubeObj::distToCube(CubeObj* cube) {
  //cout << "  current pos is " << x << ", " << y << ", " << z << endl;
  //cout << "  cube    pos is " << cube->getX() << ", " << cube->getY() << ", " << cube->getZ() << endl;
  int deltaX = x-cube->getX();
  int deltaY = y-cube->getY();
  int deltaZ = z-cube->getZ();
  //cout << "  deltas are " << deltaX << "," << deltaY << "," << deltaZ << endl;
  int theSqrt = sqrt((double)(sqrt((double)(deltaX*deltaX + deltaZ*deltaZ))*sqrt((double)(deltaX*deltaX + deltaZ*deltaZ))+deltaY*deltaY));
  //cout << "  the sqrt is " << theSqrt << endl;
  return theSqrt;
}

// Change is relative positioning
void CubeObj::changeX(int n) { x += n; }
void CubeObj::changeY(int n) { y += n; }
void CubeObj::changeZ(int n) { z += n; }
void CubeObj::changePos(int n, int o, int p) { x += n; y += o; z += p; }

// Shorthand for changePosTowards but with a target object
void CubeObj::changePosTowards(CubeObj* target, double delta) {
  changePosTowards(target->getX(),target->getY(),target->getZ(),delta);
}

// Find the differences, find the ultimate hyp, and move everything by that ratio
void CubeObj::changePosTowards(int tX, int tY, int tZ, double delta) {
  
  // Get initial differences
  int n = x - tX;
  int o = y - tY;
  int p = z - tZ;
  //cout << "at (" << n << ", " << o << ", " << p << ")" << endl;

  // Find ultimate hyp and ratio
  float hypXZ  = sqrt((double)(n*n + p*p));
  float hypXYZ = sqrt((double)(hypXZ*hypXZ + o*o));
  double ratio = delta*(-1.0)/hypXYZ;
  //cout << "Finding hypXZ as " << hypXZ << " and hypXYZ as " << hypXYZ << " and ratio as " << ratio << endl;

  //cout << "hypXYZ is " << hypXYZ << " where delta is " << delta << endl;
  // sometimes NaN
  if (!(ratio != ratio)) {
    // Then move everything. If far enough, move normally
    if (delta <= hypXYZ) {
      changeX(n*ratio);
      changeY(o*ratio);
      changeZ(p*ratio);
    // If too close, just land on it
    } else {
      changeX(-n);
      changeY(-o);
      changeZ(-p);
    }
    //cout << "Change by (" << n*ratio << ", " << o*ratio << ", " << p*ratio << ")" << endl;
  }
}

// SetMomentum is absolute momentum
void CubeObj::setMomentumX(int n) { momentumX = n * movementSpeed / movementDivision; }
void CubeObj::setMomentumY(int n) { momentumY = n * movementSpeed / movementDivision; }
void CubeObj::setMomentumZ(int n) { momentumZ = n * movementSpeed / movementDivision; }

// Move is relative momentum
void CubeObj::moveX(float n) {
  momentumX += n * movementSpeed / movementDivision;
  // NEWDELETEME: cout << "momentumX = " << momentumX << endl;
  toldToMove = (n != 0);
}
void CubeObj::moveY(float n) {
  momentumY += n * jumpSpeed / movementDivision;
  toldToMove = (n != 0);
}
void CubeObj::moveZ(float n) {
  momentumZ += n * movementSpeed / movementDivision;
  // NEWDELETEME: cout << "momentumZ += " << n << " * " << movementSpeed << " / " << movementDivision << " = " << n*movementSpeed << " / " << movementDivision << " = " << momentumZ << endl;
  toldToMove = (n != 0);
}
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
  neighborsSet = true;
  neighbors[0] = x1;
  neighbors[1] = x2;
  neighbors[2] = y1;
  neighbors[3] = y2;
  neighbors[4] = z1;
  neighbors[5] = z2;
}
bool* CubeObj::getNeighbors() { return neighbors; }
// If neighbors on both sides for one dimension
bool CubeObj::isColumn() {
  return (neighbors[0]&&neighbors[1])||(neighbors[2]&&neighbors[3])||(neighbors[4]&&neighbors[5]);
}
// If neighbors on both sides for two dimensions
bool CubeObj::isWall() {
  return neighborsSet
       &&((neighbors[0]&&neighbors[1])&&(neighbors[2]&&neighbors[3]))
       ||((neighbors[0]&&neighbors[1])&&(neighbors[4]&&neighbors[5]))
       ||((neighbors[2]&&neighbors[3])&&(neighbors[4]&&neighbors[5]));
}

// If neighbors on both sides for two dimensions, one must be vertical (2 & 3)
bool CubeObj::isVertWall() {
  return neighborsSet
       &&(((neighbors[0]&&neighbors[1])&&(neighbors[2]&&neighbors[3]))
       || ((neighbors[2]&&neighbors[3])&&(neighbors[4]&&neighbors[5])));
}

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
int CubeObj::getMaterial() { return material; }
// Set the material, for coloring
void CubeObj::setMaterial(int s) { material = s; }