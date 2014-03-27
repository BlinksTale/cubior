/*
 * cubeObj.cpp
 * by Brian Handy
 * 2/11/12
 * Represents any cube in the game (obstacles, items and players)
 */

#include "cubeObj.h"
#include "cameraObj.h"
#include "gameplay.h"

//#include <iostream>
#include <cmath> // for atan, max()/min()
#define _USE_MATH_DEFINES // for M_PI
#include <math.h> // for M_PI
#include <vector>

using namespace std;

CubeObj::CubeObj() {
  
  // FPS Rate in use?
  fpsRateEnabled = true;

  // by default, not a duplicate neighbor
  // (so you will by default be visible)
  duplicateNeighbor = false;

  // Friction for all techniques
  newFriction = 2.5;//2.35;
  friction =  2 ;
  direction = 0.0;
  toldDirection = 0.0;
  strength = 0.0;

  // And movement limits & ratios
  maxSpeed = 25 ;
  movementSpeed = 0.4;//0.3;
  minFalling = 10;
  falling = 0;

  // material default
  material = 0;
  
  // usually not a player or camera
  playerStatus = false;
  cameraStatus = false;
  // Nor an invisible wall
  invisible = false;
  // And not starting with any history of input commands
  toldToMove = false;
  lastToldToMove = false;

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
  toldToMoveX = false;
  toldToMoveY = false;
  toldToMoveZ = false;
  toldToMoveXDist = 0;
  toldToMoveYDist = 0;
  toldToMoveZDist = 0;
  
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
  justHitPlayer = false;
  landedOn = NULL;
  landedOnCount = 0;
  landedOnOnline = false; // whether locked to an online landedOn status or not
  
  // World vars
  gravity = getGravity();
  neighborsSet = false;
}

void CubeObj::tick() {
  //cout << "One cube loop" << endl;
  //if (playerStatus) { cout << "Start of player cube loop momentumZ " << momentumZ << endl; }
  //cout << "Bgn Strength:  " << strength << " and Direction: " << direction << endl;
  //cout << "TICK CALLED";
  //if (toldToMove) { cout << " and TOLD TO MOVE" << endl; }
  //cout << endl;
  // don't move if frozen
  //cout << "OneB_01 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  
  //if (isPlayer()) { cout << "Start MomentumX " << momentumX << " and MomentumZ " << momentumZ << endl; }
  if (!locked && !permalocked) {
    //if (playerStatus) { cout << "pre locked etc momentumZ " << momentumZ << endl; }
  
    if (collision) {
      setCollision(false);
      justHitPlayer = false;
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
    //if (playerStatus) { cout << "pre max speed momentumZ " << momentumZ << endl; }
  
    // cap on toldToMove
    /*if (toldToMove) {
      float str = sqrt(toldToMoveXDist*toldToMoveXDist+toldToMoveZDist*toldToMoveZDist);
      float dir = 0;
      if (toldToMoveXDist != 0.0) {
        dir = tan(toldToMoveZDist/toldToMoveXDist);
      } else {
        dir = M_PI/2+toldToMoveZDist/abs(toldToMoveZDist)*M_PI/2;
      }
      if (toldToMoveXDist < 0.00000000) { dir += M_PI; }
      cout << "str is " << str << endl;
      if (str > 0.8) { str = 0.8; }
      cout << "and is " << str << endl;
      toldToMoveXDist = str*cos(dir);
      toldToMoveZDist = str*sin(dir);
      cout << "Old for toldis " << (sqrt(toldToMoveXDist*toldToMoveXDist+toldToMoveZDist*toldToMoveZDist)) << endl;
    }*/

    // cap momentum on ground
    if (momentumX > maxSpeed) { momentumX = maxSpeed; }
    if (momentumX < -maxSpeed) { momentumX = -maxSpeed; }
    if (momentumY > maxFall) { momentumY = maxFall; }
    if (momentumY < -maxFall) { momentumY = -maxFall; }
    if (momentumZ > maxSpeed) { momentumZ = maxSpeed; }
    if (momentumZ < -maxSpeed) { momentumZ = -maxSpeed; }
    //if (momentumX != 0) { cout << "X " << momentumX << endl; }
    // Then move by that momentum
    if (abs(momentumX) > 0.00001) {
      if (momentumX > 0.00000) {
        // lesser of two positives
        x += min( 1*(float)maxMovement,(float)momentumX*myFpsRate()); 
      } else {
        // greater of two negatives
        x += max(-1*(float)maxMovement,(float)momentumX*myFpsRate()); 
      }
    } else {
      // temp fix due to odd snap when this is done *before* sending keypresses
      // otherwise, would simply not send move commands that are this small
      momentumX = 0.0000000000;
    }
    //if (momentumY != 0) { cout << "Y " << momentumY << endl; }
    if (abs(momentumY) > 0.00001) {
      if (momentumY > 0.00000) {
        y += min( 1*(float)maxMovement,(float)momentumY*myFpsRate());
      } else {
        y += max(-1*(float)maxMovement,(float)momentumY*myFpsRate());
      }
    } else {
      momentumY = 0.0000000000;
    }
    //if (momentumZ != 0) { cout << "Z " << momentumZ << endl; }
    if (abs(momentumZ) > 0.00001) {
      if (momentumZ > 0.00000) {
        z += min( 1*(float)maxMovement,(float)momentumZ*myFpsRate());
      } else {
        z += max(-1*(float)maxMovement,(float)momentumZ*myFpsRate());
      }
    } else {
      momentumZ = 0.0000000000;
    }

    if (toldToMove) {
      float toldDir = atan(toldToMoveXDist*1.0/toldToMoveZDist);
      if (toldToMoveZDist < 0.00000000) { toldDir += M_PI; }
      float str = sqrt((float)(toldToMoveXDist*toldToMoveXDist) + (float)(toldToMoveZDist*toldToMoveZDist));
      // Keep a cap on it!
      if (str > maxSpeed) {
        str = maxSpeed;
      }
      // Don't always show yourself changing directions,
      // only if some real force was put into it
      if (str >= 0.125) {
        toldDirection = toldDir;
      }
      // Then move the dist you were instructed to go
      if (toldToMoveX) {
        bool dirConflict = (momentumX*toldToMoveXDist<0);
        momentumX += toldToMoveXDist;//*(1+dirConflict);
        toldToMoveX = false;
      }
      if (toldToMoveY) {
        bool dirConflict = (momentumY*toldToMoveYDist<0);
        momentumY += toldToMoveYDist;//*(1+dirConflict);
        toldToMoveY = false;
      }
      if (toldToMoveZ) {
        bool dirConflict = (momentumZ*toldToMoveZDist<0);
        momentumZ += toldToMoveZDist;//*(1+dirConflict);
        toldToMoveZ = false;
      }
    }

    // apply friction if on the ground (universal regardless of toldToMove)
    if (toldToMove || grounded && (abs(momentumX) > 0.00001 || abs(momentumZ) > 0.00001)) {
      // change x/z into dir and str, then apply friction omnidirectionally/in proportion
      // and convert back to apply to momentum
      // And since moving, update your direction!
      // (toldDir being which direction you have told to go in, vs what you are going in)
      float dir = atan(momentumX*1.0/momentumZ);
      if (momentumZ < 0.00000000) { dir += M_PI; }
      float str = sqrt((float)(momentumX*momentumX) + (float)(momentumZ*momentumZ));
      // Keep a cap on it!
      if (str > maxSpeed) {
        str = maxSpeed;
      }
      // Can only apply friction if not being moved by player
      // (thus we can still use this section to cap max speed)
      // can also apply friction if fighting current direction
      if (!toldToMove){//getDirectionConflict()) {
        if (str > newFriction*myFpsRate()) {
          // only use half the friction for a directional conflict though
          str -= newFriction*myFpsRate();
        } else if (str != 0.0) {
          str = 0.0;
        }
      }
      // Similarly
      if (momentumX != 0.0 || momentumZ != 0.0) {
        if (str > newFriction*myFpsRate()) {
          // only use half the friction for a directional conflict though
          str -= newFriction*myFpsRate()*(getDirectionConflictSeverity()/4.0);
        }
      }
      // And then universal friction, for kicks
      // (helps the feel a lot actually, need a more official form of this)
      str -= newFriction*myFpsRate()/16.0;

      // Don't have negative strength after all that friction!
      if (str < 0.0) { str = 0.0; }

      // Now update direction... maybe, if str was strong enough
      //if (str >= newFriction) {
        direction = dir;
      //}
      // and always update strength
      if (strength != str) {
        strength = str;
      } 
      // then lastly, transfer back to momentumX and Z
      momentumX = sin(direction)*strength;
      momentumZ = cos(direction)*strength;
    }
      
    // Finally, update if toldToMove or not
  	if (toldToMove) {
      // And if toldToMove, also move that dist
      lastToldToMove = true;
      toldToMove = false; // only here because it was true, must make it false for next time!

      //cout << "Hyp for toldis " << (sqrt(toldToMoveXDist*toldToMoveXDist+toldToMoveZDist*toldToMoveZDist)) << endl;
      //cout << "Hyp for x/z is " << (sqrt(momentumX*momentumX+momentumZ*momentumZ)) << " vs " << strength << endl;
	  } else {
      // Not told to move? Then, we have not done so recently.
      lastToldToMove = false;
    } 

    calculateDiff();
    
  // Can lose all momentum on locking if bool is set
  } else if (isMoving() && loseMomentumOnLock) { freeze(); }
  //if (playerStatus) { cout << "Post main momentumZ " << momentumZ << endl; }
  
  //cout << "OneB_06 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

  // Momentum loss & gravity apply if free or locked-and-loseMomentum-on-Lock
  if ((!locked || loseMomentumOnLock) && !permalocked) { fall(); }
  //if (playerStatus) { cout << "Post fall momentumZ " << momentumZ << endl; }
  
  //cout << "OneB_07 player at "<<x<<", "<<y << ", "<<z<<"\t with momentum "<<momentumX<<", "<<momentumY<<", "<<momentumZ<<endl;  

  // And at the end, update your relation to the landedOn
  if (updateLandedOnPos()) {
    // Any changes? Also update your directional diff
    landedOnDirectionDiff = direction - landedOn->getDirection();
    landedOnToldDirectionDiff = toldDirection - landedOn->getToldDirection();
  }
}

float CubeObj::getStrength() {
  return strength;
}
float CubeObj::getLandedOnDirectionDiff() {
  return landedOnDirectionDiff;
}
void CubeObj::setLandedOnDirectionDiff(float newDiff) {
  landedOnDirectionDiff = newDiff;
}

float CubeObj::getDirection() {
  float result = direction;
  if (getLandedOnCount()>0) {
    result = landedOn->getDirection() + landedOnDirectionDiff;
  }
  return result;
}
void CubeObj::setDirection(float f) {
  direction = f;
}
// Return the last direction you were told (by keyboard/joystick) to move in
float CubeObj::getToldDirection() {
  float result = toldDirection;
  // May as well keep the impact of the dir of those you stand on, no harm right?
  if (getLandedOnCount()>0) {
    result = landedOn->getToldDirection() + landedOnToldDirectionDiff;
  }
  return result;
}
void CubeObj::setToldDirection(float f) {
    toldDirection = f;
}
void CubeObj::setToldDirectionAverage(float f) {
    toldDirection = (f + toldDirection)/2.0;
}

// Return direction by camera's angle system, and in degrees
int CubeObj::getCamDirection() {
  // Start with old function
  float result = getDirection(); // in radians and offset by PI still
  // Rotate to Camera's angle system (not sure why different, FIXME someday?)
  result = result + M_PI;

  // Convert to degrees
  int intResult = (int)((result * 360)/(2*M_PI));

  return intResult;
}

int CubeObj::getLandedOn() {
  if (landedOn != NULL && landedOn->isPlayer()) {
    return ((CubiorObj*)landedOn)->getPlayerNum();
  } else {
    return -1;
  }
}

void CubeObj::setLandedOn(int landedOnNum) {
  if (landedOnNum >= 0 && landedOnNum < cubiorCount) {
    landOn(getPlayer(landedOnNum));
    landedOnOnline = true;
  } else if (landedOnOnline) {
    landedOnOnline = false;
    resetLandedOn();
  } 
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
  falling++;
  momentumY -= gravity*myFpsRate();
  doubleLastGrounded = lastGrounded;
  lastGrounded = grounded;
  grounded = false;
  //cout << "fall lG is " << lastGrounded << " and g is " << grounded << endl;
  if (getNotGrounded() && landedOn != NULL) {
    momentumX += landedOn->getMomentumX(); // maybe *fpsRate()?
    //momentumY += landedOn->getMomentumY();
    momentumZ += landedOn->getMomentumZ(); // maybe *fpsRate()?
    direction = landedOn->getDirection() + landedOnDirectionDiff;
    toldDirection = landedOn->getToldDirection() + landedOnToldDirectionDiff;
    if (!landedOnOnline) {
        resetLandedOn();
    }
  } // not on a player anymore!
}

void CubeObj::resetLandedOn() {
  landedOn = NULL;
  landedOnCount = 0;
  landedOnDirectionDiff = 0.0;
  landedOnToldDirectionDiff = 0.0;
}

// Act as if you landed on ground
void CubeObj::land() {
  if (falling > minFalling) {
    justLandedBool = true;
    justLandedInt = falling;
  }
  falling = 0;
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
  if (!landedOnOnline && landedOn != c && c->getLandedOnCount() < cubiorCount) {
    landedOn = c;
    landedOnCount = c->getLandedOnCount() + 1;
    landedOnDirectionDiff = direction - landedOn->getDirection();
    landedOnToldDirectionDiff = toldDirection - landedOn->getToldDirection();
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

int CubeObj::getLandedOnX() { return landedOnX; }
int CubeObj::getLandedOnY() { return landedOnY; }
int CubeObj::getLandedOnZ() { return landedOnZ; }
void CubeObj::setLandedOnX(int newVal) { landedOnX = newVal; }
void CubeObj::setLandedOnY(int newVal) { landedOnY = newVal; }
void CubeObj::setLandedOnZ(int newVal) { landedOnZ = newVal; }
void CubeObj::setLandedOnPos(int newX, int newY, int newZ) {
    landedOnX = (newX);
    landedOnY = (newY);
    landedOnZ = (newZ);
}
void CubeObj::setLandedOnPosAverage(int newX, int newY, int newZ) {
    landedOnX = (newX + landedOnX)/2;
    landedOnY = (newY + landedOnY)/2;
    landedOnZ = (newZ + landedOnZ)/2;
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

// isCamera returns whether just a normal block or a camera
bool CubeObj::isCamera() {
  return cameraStatus;
}

void CubeObj::setCameraStatus(bool b) {
  cameraStatus = b;
}

/*void CubeObj::setCamera(CameraObj* c) {
  camera = c;
}

CameraObj* CubeObj::getCamera() {
  return camera;
}*/

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
void CubeObj::collisionEffect(CubeObj* c) {
    if (c->isPlayer()) {
      // distToCube returns zero if no collisions. Odd, but use for now. Fixme later
      if (this->distToCube(c) != 0.0f) {
        justHitPlayer = true;
        posAverageBias = 1.0f;
      }
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
bool CubeObj::getStillGrounded() { return lastGrounded || grounded || doubleLastGrounded; }
bool CubeObj::getStillJumping() { return !lastGrounded && !grounded && !doubleLastGrounded; }
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
// This one returns a volume out of 100
int CubeObj::justLanded() {
  int result = justLandedInt;
  if (justLandedInt > 0) {
    justLandedInt = 0;
  }
  return result;
}
int CubeObj::justSkidded() {
  bool directionConflict = getDirectionConflict();
  bool conflict = getStillGrounded() && directionConflict && !lastDirectionConflict;
  lastDirectionConflict = directionConflict;
  int result = 0;
  if (conflict) {
    result = getDirectionConflictSeverity()*getDirectionConflictSeverity()*100;
  }
  return result;
}
int CubeObj::justFlipped() {
  bool directionFlip = getDirectionConflict();
  bool conflict = getStillJumping() && directionFlip && !lastDirectionFlip;
  lastDirectionFlip = directionFlip;
  int result = 0;
  if (conflict) {
    result = getDirectionConflictSeverity()*getDirectionConflictSeverity()*100;
  }
  return result;
}
int CubeObj::justMoved() {
  bool conflict = getStillGrounded() && (momentumX != 0 || momentumZ != 0);
  int result = lastMoved;
  if (conflict) {
    result = strength*2;
  } else if (result != 0) {
    result = 0;
  }
  lastMoved = result;
  return result;
}

// Set is absolute positioning
void CubeObj::setX(int n) { x = n; }
void CubeObj::setY(int n) { y = n; }
void CubeObj::setZ(int n) { z = n; }
void CubeObj::setPos(int n, int o, int p) { x = n, y = o, z = p; }
void CubeObj::setPosAverage(int n, int o, int p) {
  // apply if not touching a player
  if (!justHitPlayer) {
    float bias = posAverageBias; // bias towards current position
    x = n*(1.0f-bias)+x*bias;
    y = o*(1.0f-bias)+y*bias;
    z = p*(1.0f-bias)+z*bias;
    
    // Finally at the right position? Reset the bias
    if (x==n && y==o && z==p) {
      if (posAverageBias != 0.9f)
        posAverageBias = 0.9f;
    } else {
      // Otherwise, need to get closer
      if (posAverageBias > 0.0f)
        posAverageBias -= 0.1f;
      else if (posAverageBias != 0.0f)
        posAverageBias = 0.0f;
    }
  }
}


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
  changePosTowards(target->getX(),target->getY(),target->getZ(),delta*myFpsRate());
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
  double ratio = delta*(-1.0)/hypXYZ*myFpsRate();
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
void CubeObj::setMomentumX(float n) { momentumX = n * movementSpeed / movementDivision; }
void CubeObj::setMomentumY(float n) { momentumY = n * movementSpeed / movementDivision; }
void CubeObj::setMomentumZ(float n) { momentumZ = n * movementSpeed / movementDivision; }

void CubeObj::setMomentum(vector<float> m) { 
  momentumX = (m.at(0) * movementSpeed / movementDivision);
  momentumY = (m.at(1) * movementSpeed / movementDivision);
  momentumZ = (m.at(2) * movementSpeed / movementDivision);
}
void CubeObj::setMomentumAverage(vector<float> m) {
    momentumX = (m.at(0) * movementSpeed / movementDivision + momentumX)/2.0;
    momentumY = (m.at(1) * movementSpeed / movementDivision + momentumY)/2.0;
    momentumZ = (m.at(2) * movementSpeed / movementDivision + momentumZ)/2.0;
}

// Just wipe out any old toldToMove commands
void CubeObj::resetToldToMove() {
  toldToMove = false;
  toldToMoveX = false;
  toldToMoveY = false;
  toldToMoveZ = false;
}

// Move is relative momentum
void CubeObj::moveX(float n) {
  float diff = n * movementSpeed*myFpsRate() / movementDivision;
  // Only update dist to move if any dist change
  if (n == n) { // not NaN
    if (toldToMoveX) {
      toldToMoveXDist += diff;
    } else {
      toldToMoveXDist = diff;
    }
  }
  //momentumX += diff;
  toldToMoveX = toldToMoveX || (n != 0.0000000 && n==n);
  toldToMove = toldToMove || (n != 0.0000000 && n==n);
}
void CubeObj::moveY(float n) {
  float diff = n * jumpSpeed*myFpsRate() / movementDivision;
  // Only update dist to move if any dist change
  if (n == n) { // not NaN
    if (toldToMoveY) {
      toldToMoveYDist += diff;
    } else {
      toldToMoveYDist = diff;
    }
  }
  //momentumY += diff;
  toldToMoveY = toldToMoveY || (n != 0.0000000 && n==n);
  toldToMove = toldToMove || (n != 0.0000000 && n==n);
}
void CubeObj::moveZ(float n) {
  float diff = n * movementSpeed*myFpsRate() / movementDivision;
  // Only update dist to move if any dist change
  if (n == n) { // not NaN
    if (toldToMoveZ) {
      toldToMoveZDist += diff;
    } else {
      toldToMoveZDist = diff;
    }
  }
  toldToMoveZ = toldToMoveZ || (n != 0.0000000 && n==n);
  toldToMove = toldToMove || (n != 0.0000000 && n==n);
  //momentumZ += diff;
}
void CubeObj::movePos(float n, float o, float p) {
  toldToMoveX = true;
  toldToMoveY = true;
  toldToMoveZ = true;
  toldToMoveXDist = n;
  toldToMoveYDist = o;
  toldToMoveZDist = p;

  //momentumX += n * movementSpeed*myFpsRate() / movementDivision;
  //momentumY += o * movementSpeed*myFpsRate() / movementDivision;
  //momentumZ += p * movementSpeed*myFpsRate() / movementDivision;
  toldToMove = toldToMove || (n != 0.0000000) || (o != 0.0000000) || (p != 0.0000000);
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
vector<float> CubeObj::getMomentum() {
    float momentumArray[] = {
        momentumX * movementDivision,
        momentumY * movementDivision,
        momentumZ * movementDivision
    };
    std::vector<float> newMomentum (momentumArray, momentumArray + sizeof(momentumArray) / sizeof(float) );

    return newMomentum;
}
int CubeObj::getMaxJump() { return maxJump; }
// How fast we are travelling on the ground
int CubeObj::getMomentumGround() {
  int momentumGround = sqrt(momentumX*momentumX+momentumZ*momentumZ);
  return momentumGround * movementDivision;
}

void CubeObj::setNeighbors(bool x1, bool x2, bool y1, bool y2, bool z1, bool z2) {
  neighborsSet = true;
  neighbors[0] = x1;
  neighbors[1] = x2;
  neighbors[2] = y1;
  neighbors[3] = y2;
  neighbors[4] = z1;
  neighbors[5] = z2;
}
// Same as neighbors, but only visible ones
void CubeObj::setVisibleNeighbors(bool x1, bool x2, bool y1, bool y2, bool z1, bool z2) {
  visibleNeighborsSet = true;
  visibleNeighbors[0] = x1;
  visibleNeighbors[1] = x2;
  visibleNeighbors[2] = y1;
  visibleNeighbors[3] = y2;
  visibleNeighbors[4] = z1;
  visibleNeighbors[5] = z2;
}
// For actual objects, so properties (eg. material/colors) can be referenced
void CubeObj::setVisibleNeighborObjects(CubeObj* x1, CubeObj* x2, CubeObj* y1, CubeObj* y2, CubeObj* z1, CubeObj* z2) {
  //visibleNeighborObjectsSet = true;
  visibleNeighborObjects[0] = x1;
  visibleNeighborObjects[1] = x2;
  visibleNeighborObjects[2] = y1;
  visibleNeighborObjects[3] = y2;
  visibleNeighborObjects[4] = z1;
  visibleNeighborObjects[5] = z2;
}
bool* CubeObj::getNeighbors() { return neighbors; }
bool* CubeObj::getVisibleNeighbors() { return visibleNeighbors; }
CubeObj** CubeObj::getVisibleNeighborObjects() { return visibleNeighborObjects; }
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
void CubeObj::applyCollisionMomentumX() { momentumX *= 0.95; }
void CubeObj::applyCollisionMomentumZ() { momentumZ *= 0.95; }
bool CubeObj::getCollision() { return collision; }

// Checks if diff between dir and toldDir by >=90 deg (but radians, so PI/2)
bool CubeObj::getDirectionConflict() {
  // lastToldToMove included since only a conflict if trying to move
  // must have some movement too
  return abs(direction - toldDirection) >= M_PI/2.0 && lastToldToMove;
}
// On a scale of 0.0 to 1.0, returns result of how far into the conflict we are
// 1.0 represents an M_PI difference, while 0.0 means they point the same way
float CubeObj::getDirectionConflictSeverity() {
  float result;
  float diff = abs(direction - toldDirection);
  if (diff > M_PI) {
    result = M_PI;
  } else if (diff < 0) {
    result = 0;
  } else {
    result = diff;
  }
  result = result/M_PI;
  return result;
}
int CubeObj::getWidth() { return 100*(1); }
int CubeObj::getHeight() { return 100*(1); }
int CubeObj::getSize(int s) { return s == 1 ? 100 : 100; } // was getHeight and getWidth but Segfaulted on Mac. Read: I need to learn virtual functions better
int CubeObj::getMaterial() { return material; }
int CubeObj::getAlternatingSpotSize() {
  return altSize;
}
bool CubeObj::getAlternatingSpot() { // return if in a checker or not
  bool alternatingSpot =( // calculate if given a dark checker spot or not
      (x<0)^((int(abs(x+1))%(altSize*2)<altSize))
    ) ^ (
      (y<0)^((int(abs(y+1))%(altSize*2)<altSize))
    )^ (
      (z<0)^((int(abs(z+1))%(altSize*2)<altSize))
    );
  return alternatingSpot;
}
// check if you have been set as a duplicate, eliminated by extended polygons
bool CubeObj::getDuplicateNeighbor() { return duplicateNeighbor; }
// Set the material, for coloring
void CubeObj::setMaterial(int s) { material = s; }
// set duplicate status (would or wouldn't be overridden by extended polygons)
void CubeObj::setDuplicateNeighbor(bool b) { duplicateNeighbor = b; }

// Custom FPS rate based on whether enabled or not
float CubeObj::myFpsRate() {
  if (fpsRateEnabled) {
    return fpsRate();
  } else {
    return 1.0;
  }
}

// Return true if recently sent a movement command (joystick or keyboard dir input)
bool CubeObj::getLastToldToMove() { return lastToldToMove; }

int CubeObj::getMaxSpeed() { return maxSpeed * movementDivision; }

void CubeObj::setInvisible(bool b) { invisible = b; }

bool CubeObj::isInvisible() { return invisible; }