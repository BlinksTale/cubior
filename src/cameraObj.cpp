/*
 * cameraObj.cpp
 * by Brian Handy
 * 3/29/12
 * Represents the game's camera location/etc.
 */

#include "cameraObj.h"
#include "gameplay.h"
#include "trackerObj.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

CameraObj::CameraObj() {
  // Pos vars
  resetPos();
  
  // Follow vars
  cameraSide = 0; // where zero is undetermined
  farthestDist = 1200;
  closestDist = 1000;
    
  idealDist = (farthestDist+closestDist)/2;
  tracker = new TrackerObj();
  followingBoth = false; // Start by just following cube, not also goal
  nearGoal = false; // inherently won't start right under the goal
  los = true; // Assume you can see the player from the sky :P
}

// Set to starting pos, start of a new level
void CameraObj::resetPos() {
  x = 0;
  y = 1000000;
  z = -1000;
  angleX = -45;
  angleY = 0;
  angleZ = 0;
  
  // start with no intended pos and full movement freedom
  foundIntendedPos = false;
  freedom = true;
}

// The most basic increment, called once per main loop/frame
void CameraObj::tick() {
  //cout << "Start camera tick" << endl;
  // If you are following one target every frame,
	if (permanentTarget) {
    // make sure to update the item that's following the target
	  tracker->tick();
    // then move the camera itself if free to do so
    if (freedom && !foundIntendedPos) {
      //cout << "Normal loop! Freedom found, no intended pos" << endl;
      follow(
        tracker->getX(),
        permanentTarget->getY(),
        tracker->getZ(),
        permanentTarget->getAngleY(),
        permanentTarget->getGrounded(),
        4
      );
    } else { // not free or have an intended pos
      cout << "You have no freedom (" << (!freedom) << ") or have found an intended position (" << foundIntendedPos << ")!" << endl;
      // if you do have a place to be or aren't allowed to move,
      if (foundIntendedPos) {
        cout << "Intended Position found, dist to cube is " << distToCube(&intendedPos) << endl;
        cout << "so intendedPos " << intendedPos.getX() << ", " << intendedPos.getY() << ", " << intendedPos.getZ() << endl;
        cout << "&  currentPos  " << x << ", " << y << ", " << z << endl;
        cout << "so it being greater than camSpeed of " << camSpeed << " is " << (distToCube(&intendedPos) > camSpeed) << endl;
        // If still not at the destination
        if (distToCube(&intendedPos) > camSpeed) {
          cout << "Moving towards intended pos" << endl;
          cout << "currentPos  " << x << ", " << y << ", " << z << endl;
          // Move towards it
          changePosTowards(&intendedPos,camSpeed);
          cout << "Move success!" << endl;
          cout << "currentPos  " << x << ", " << y << ", " << z << endl;
        } else {
          cout << "No need to move, already here" << endl;
          cout << "currentPos  " << x << ", " << y << ", " << z << endl;
          x = intendedPos.getX();
          y = intendedPos.getY();
          z = intendedPos.getZ();
          cout << "currentPos  " << x << ", " << y << ", " << z << endl;
          // You're there! Stop trying.
          foundIntendedPos = false;
          // currently disabled to try and find a technique without this
          //freedom = false;
          cout << "intendedPos and freedom set to false" << endl;
        }
      }
      // No matter what, since you have a permanent target,
      // always look at the player still
      lookAtPlayer(
        tracker->getX(),
        permanentTarget->getY(),
        tracker->getZ(),
        permanentTarget->getAngleY(),
        permanentTarget->getGrounded(),
        4);
      
      cout << "intendedPos " << intendedPos.getX() << ", " << intendedPos.getY() << ", " << intendedPos.getZ() << endl;
      cout << "currentPos  " << x << ", " << y << ", " << z << endl;
      cout << "targetPos   " << permanentTarget->getX() << ", " << permanentTarget->getY() << ", " << permanentTarget->getZ() << endl;

      // FIXME: Pretty darn sure this whole chunk I just added must be broken. groundDistToPlayer()? Really?
      /*positionByAngles(
        tracker->getX(),
        tracker->getZ(),
        100,
        groundDistToPlayer(),
        permanentTarget->getAngleY(),
        4);*/
    }
  }
  //cout << "End camera tick" << endl;
}

// Used to permanently set a target to always follow
void CameraObj::alwaysFollow(CubeObj* target, CubeObj* targetGoal) {
  permanentTarget = target;
  permanentTargetGoal = targetGoal;
  lastLandedY = target->getY();
  lastLanded = target->getLanded();
  x = target->getX()-camHeight;
  y = target->getY()+50*camHeight;
  z = target->getZ()-camHeight;
  tracker->setTarget(permanentTarget);
  follow(
      tracker->getX(),
      permanentTarget->getY(),
      tracker->getZ(),
      permanentTarget->getAngleY(),
      permanentTarget->getGrounded(),
      1
    );
}

// Find how close player is to their goal,
// if close enough we'll use this to look at both at once
int CameraObj::distToGoal() {
  int deltaX = permanentTarget->getX()-permanentTargetGoal->getX();
  int deltaZ = permanentTarget->getZ()-permanentTargetGoal->getZ();
  return sqrt(deltaX*deltaX + deltaZ*deltaZ);
}

// Find how close player is to camera
int CameraObj::distToPlayer() {
  int deltaX = x-permanentTargetGoal->getX();
  int deltaY = y-permanentTargetGoal->getY();
  int deltaZ = z-permanentTargetGoal->getZ();
  return sqrt(sqrt(deltaX*deltaX + deltaZ*deltaZ)*sqrt(deltaX*deltaX + deltaZ*deltaZ)+deltaY*deltaY);
}

// Find how high player is to camera
int CameraObj::heightToPlayer() {
  int deltaY = y-permanentTargetGoal->getY();
  return abs(deltaY);
}

// Find how close player is to camera on X,Z
int CameraObj::groundDistToPlayer() {
  return groundDistTo(permanentTargetGoal->getX(),permanentTargetGoal->getZ());
}

// Find how close camera is to some X,Z
int CameraObj::groundDistTo(int a, int c) {
  int deltaX = x-a;
  int deltaZ = z-c;
  return sqrt(deltaX*deltaX + deltaZ*deltaZ);
}

// Ground angle to goal from camera
float CameraObj::angleToGoal() {
  int deltaX = x-permanentTargetGoal->getX();
  int deltaZ = z-permanentTargetGoal->getZ();
  return deltasToDegrees(deltaX, deltaZ) + (deltaZ<0)*180;
}

// Ground angle to goal from player/permanentTarget
float CameraObj::angleBetweenPlayerAndGoal() {
  int deltaX = permanentTarget->getX()-permanentTargetGoal->getX();
  int deltaZ = permanentTarget->getZ()-permanentTargetGoal->getZ();
  return deltasToDegrees(deltaX, deltaZ) + (deltaZ<0)*180;
}

// Look at the point halfway between the player and their goal
void CameraObj::betweenPlayerAndGoal() {
  int playerX = permanentTarget->getX();
  int playerZ = permanentTarget->getZ();
  int goalX = permanentTargetGoal->getX();
  int goalZ = permanentTargetGoal->getZ();

  int deltaX = playerX-goalX;
  int deltaZ = playerZ-goalZ;

  if (distToGoal() < goalRange) {

    angleY = deltasToDegrees(x-goalX, z-goalZ);
  }
}

// Check if you can even get to goal vertically
bool CameraObj::goalWithinJumpRange() {
  return abs(permanentTarget->getY() - permanentTargetGoal->getY()) < 500;
}

// Check if you can even get to goal horizontally
bool CameraObj::goalWithinDistRange() {
  return distToGoal() < goalRange;
}

// Check if you cannot even get to goal horizontally
bool CameraObj::goalOutsideDistRange() {
  return distToGoal() > goalRange * 1.25;
}

// Check if you are right underneath the goal
bool CameraObj::goalWithinNearRange() {
  return distToGoal() < goalRange * 0.25;
}

// Check if you aren't right underneath the goal
bool CameraObj::goalOutsideNearRange() {
  return distToGoal() > goalRange * 0.35;
}

// Convert a diff in two dimensions to an angle
float CameraObj::deltasToDegrees(int opp, int adj) {
  float PI = 3.14159;
  float result = (adj!=0) ? atan(opp/(adj*1.0)) : (-2*(opp > 0)+1)*PI/2.0;

  // This accounts for how atan only covers the top half of the circle for degrees
  // cout << "result at this point is " << result << endl;

  // This code was supposed to fix atan's wraparound issues... ended up causing them?
  // Commented out and saved just in case things go horribly wrong somehow.
  //if (opp < 0) { result += (-2*(adj<0)+1)*PI-result; } // true = 1, false = 0

  // Then convert radians to degrees
  result *= 360.0/(2.0*PI);
  return result;
}


// When camera starts following both, decide from which angle
float CameraObj::findFollowingBothSide(float angleYToBe, float angleY) {
  float atg = angleToGoal();
  angleY = matchRangeOf(angleY,atg);
  float angleDiff = atg - angleY;
  return angleDiff > 0 ? -1 : 1;
}

// Gives intendedDist for looking at both goal and follow target (player)
int CameraObj::lookAtBoth(int a, int c, int intendedDist) {
    // FIXME: For the presentation: getX/getZ aren't working but 0 is, so... whatever works for now
    // was originally designed to take in intendedDist... not sure why I don't have that now
    // must FIXME for intendedDist later as well!
    a = (a*2 + (0 + a/2))/3;//(a + permanentTarget->getX()*3) / 4;
    c = (c*2 + (0 + c/2))/3;//(c + permanentTarget->getZ()*3) / 4;
    return farthestDist * (goalRange + distToGoal()/10) / goalRange;
}

void CameraObj::checkExtremeCatchup(int distToPlayer, int a, int c, int num, int den) {
  if (distToPlayer > 3*farthestDist) {
    x = (a+farthestDist*sin(angleY)*num + x)/den;
    z = (c+farthestDist*cos(angleY)*num + z)/den;
    y = ((camHeight*2-camHeight*distToPlayer/(farthestDist))*num + y)/den;
  }
}

/*
 * IntendedDist is how far to be from the player.
 * If following two things (goal and player),
 * just move back far enough to see both.
 */
int CameraObj::findIntendedDist(int a, int c) {
  int intendedDist = farthestDist;
  if (followingBoth) {
    intendedDist = lookAtBoth(a,c,intendedDist);
  }
  return intendedDist;
}

float CameraObj::findTargetY(int playerY, bool landed) {
  // Look at vertical position halfway between current and last ground landing
    // (let's us see a bit higher when jumping)
    float targetY = (playerY+lastLandedY)/2;
    // If you are on the ground, or were just on the ground a moment ago,
    if (landed || lastLanded) {
      // then your position is the vertical point to aim at
      targetY = playerY;
      lastLandedY = playerY;
    }
    // And then with every new step, update lastLanded
    // (because currently it alternates between landed and not when actually landed)
    lastLanded = landed; 
    return targetY;
}
// Do the following itself of the camera's target
void CameraObj::follow(int a, int b, int c, int playerAngle, bool landed, int strictness) {

  // Basically, trying to find right angle, then distance camera appropriately
  
  // For smoothing purposes
  int num = 10, den = 11; 
  // and measuring dist
  int distToPlayer = groundDistTo(a,c);
  // (if really too far though, catch up to player)
  checkExtremeCatchup(distToPlayer,a,c,num,den);
  
  /* Distance deciding segment */
  int intendedDist = findIntendedDist(a,c);
  
  // so the camera doesn't go below a certain point
  // FIXME: Actually, I think these lines aren't even used - so, commented out
  //int newDist = distToPlayer > farthestDist ? farthestDist : 
    //(distToPlayer < closestDist ? closestDist : distToPlayer);
  // Set camera at a Y height between its current y and the last landed + some height - that height by dist/farthest ratio
  int intendedY = (lastLandedY+camHeight*2-camHeight*distToPlayer/(farthestDist));
  y = (intendedY+y*num)/den;
  // FIXME: This is good for not going underground, but causes jumpiness
  if (landed) { if (y<b+200) { y = (b+200+y*num)/den; }
  } else if (y<b+200) { y = (b+200+y*num)/den; } // was "if y<lastLandedY+200" but caused jerkiness, so, meh
  
  /*
   * Y position is figured out 
   * Now move on to the angles
   */
  if (b<y) { // Don't look up if player above you, makes camera flip over to backside
    lookAtPlayer(a,b,c,playerAngle,landed,strictness);
  }
  /* Just figure out X & Z positions now */
  
  // Finally, implement movement for the camera in its new facing direction
  positionByAngles(a,c,intendedDist,distToPlayer,angleY,strictness);
	
}

// Bare basics of any normal tick/update: look at the player
void CameraObj::lookAtPlayer(int a, int b, int c, int playerAngle, bool landed, int strictness) {
  
    int num = 10, den = 11; 
    /* Vertical aiming segment */
    float targetY = findTargetY(b, landed);
    // quick temp var for the triangle between ground hyp and y diff
    float vertAtan = deltasToDegrees(groundDistTo(a,c),y-targetY);  
    // angleXToBe is how far down to be looking, from the camera, to the player
    float angleXToBe =  vertAtan != 0 ? 270 + vertAtan : angleX;

    /* Horizontal aiming segment */
    // The 1.0 is necessary for floating point division, as a/x/c/z are all ints
    float angleYToBe = ((c-z<=0?0:180)+deltasToDegrees(a-x,c-z));

    // Figure out follow-just-the-player mode
    if (!followingBoth) {
      // Only point the way the player faces if you're moving
      cout << "momentum x/z > 10 = " << (abs(permanentTarget->getMomentumX())>10 || abs(permanentTarget->getMomentumZ())>10) << endl;
      if (abs(permanentTarget->getMomentumX())>10 || abs(permanentTarget->getMomentumZ())>10) {
        angleYToBe = followOne(angleYToBe, playerAngle, num, den);
      }
    // Otherwise, watch the player *and* the goal
    } else {
      angleYToBe = followBoth(angleYToBe);
    }

    // Then update the final angles themselves
    angleY = matchRangeOf(angleY, angleYToBe);
    angleY += -(angleY-angleYToBe)/strictness;
    angleX += -(angleX-angleXToBe)/strictness;
    angleZ = 0; // Generally, don't want to change this - causes disorientation
}

// Final step of follow, adjust position to match camera angles
void CameraObj::positionByAngles(int a, int c, int intendedDist, int distToPlayer, float angleY, int strictness) {
  int viewingDist = (intendedDist + 3*distToPlayer)/4;
  int xToBe = a+viewingDist*sin(angleY*2*3.14159/360);
  int zToBe = c+viewingDist*cos(angleY*2*3.14159/360);
  x += -(x-xToBe)/strictness;
  z += -(z-zToBe)/strictness;
}

float CameraObj::followOne(float oldYToBe, int playerAngle, int num, int den) {
  
  float angleYToBe = oldYToBe;
  
  // be inclined towards angle player is facing if following
  if ( withinRangeOf(tracker->getAngleY(),permanentTarget->getAngleY(),45) ) {
    playerAngle = matchRangeOf(playerAngle,angleYToBe);
    // Camera only turns if player going mostly away from camera
    if ((playerAngle < angleY + 125) && (playerAngle > angleY - 125)) {
      angleYToBe = (angleYToBe*num + playerAngle)/den;
    }
  }

  // But if within goal range, *change* modes to look at player AND the goal
  if (goalWithinDistRange() && goalWithinJumpRange()) {
    // Figure out which side to follow from
    cameraSide = findFollowingBothSide(angleYToBe, angleY);
    followingBoth = true;
  }
  
  return angleYToBe;
}

// Use this if following player and goal both
float CameraObj::followBoth(float oldYToBe) {

  float angleYToBe = oldYToBe;
  // if outside range...
  if (goalOutsideDistRange() || !goalWithinJumpRange()) {
    // stop following goal if no longer near it
    followingBoth = false;
  // if within range...
  } else {
    // ...but not too near...
    if (goalOutsideNearRange()) {
      // If last step you were near the goal still
      if (nearGoal) {
        nearGoal = false;
        // set a new camera side! May have moved to other side of cube
        cameraSide = findFollowingBothSide(angleYToBe, angleY);
      }
      int viewingAngle = angleBetweenPlayerAndGoal() + 90*cameraSide;
      viewingAngle = matchRangeOf(viewingAngle, angleYToBe);
      angleYToBe = (angleYToBe*3 + viewingAngle)/4;
    // ...but if you ARE too near...
    } else if (!nearGoal) {
      nearGoal = true;
    }
  }
  
  return angleYToBe;
}
// See if y1 within delta of y2
bool CameraObj::withinRangeOf(int y1, int y2, int delta) {
  matchRangeOf(y1,y2);
  return (y1 < y2+delta && y1 > y2-delta);
}

// Set y1 to within 180 of y2
float CameraObj::matchRangeOf(float y1, float y2) {
  if (y1 - y2 > 180) { y1 -= 360; }
  else if (y2 - y1 > 180) { y1 += 360; }
  return y1;
}

// Set y1 to within 180 of y2
float CameraObj::smoothMatchRangeOf(float y1, float y2) {
  if (y1 < 0 && y2 > 180) { y1 += 360; }
  else if (y1 > 180 && y2 < 0) { y1 -= 360; }
  return y1;
}

void CameraObj::setIntendedPos(CubeObj* c) {
  intendedPos.setPos(c->getX(),c->getY(),c->getZ());
  foundIntendedPos = true;
}

bool CameraObj::getFoundIntendedPos() { return foundIntendedPos; }
void CameraObj::setFoundIntendedPos(bool b) { foundIntendedPos = b; }
void CameraObj::disableIntendedPos() { foundIntendedPos = false; }

// Getters
int CameraObj::get(int s) { return s == 0 ? x : s == 1 ? y : z; }
int CameraObj::getX() { return x; }
int CameraObj::getY() { return y; }
int CameraObj::getZ() { return z; }
float CameraObj::getAngle(int s) {
  return s == 0 ? angleX : s == 1 ? angleY : angleZ;
}
float CameraObj::getAngleX() { return angleX; }
float CameraObj::getAngleY() { return angleY; }
float CameraObj::getAngleZ() { return angleZ; }

// Set is absolute positioning
void CameraObj::setPos(int n, int o, int p) { x = n, y = o, z = p; }
void CameraObj::setX(int n) { x = n; }
void CameraObj::setY(int n) { y = n; }
void CameraObj::setZ(int n) { z = n; }
void CameraObj::setAngle(float n, float o, float p) {
  angleX = n, angleY = o, angleZ = p;
}
void CameraObj::setAngleX(float n) { angleX = n; }
void CameraObj::setAngleY(float n) { angleY = n; }
void CameraObj::setAngleZ(float n) { angleZ = n; }

// Regarding camera's ability to move on its own, for follow, vs. set placement
void CameraObj::setFreedom(bool b) { freedom = b; }
bool CameraObj::getFreedom() { return freedom; }

// Line of Sight - returns if view to player is clear or not
void CameraObj::setLOS(bool b) {
  //cout << "LOS SET TO " << b << "!!!!!!!!!!!!" << endl;
  los = b; 
}
bool CameraObj::getLOS() { return los; }
bool CameraObj::getVisibility() { return los; }
CubeObj* CameraObj::getPermanentTarget() { return permanentTarget; }
