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

// Set to starting pos
void CameraObj::resetPos() {
  x = 0;
  y = 1000000;
  z = -1000;
  angleX = -45;
  angleY = 0;
  angleZ = 0;
}

// The most basic increment, called once per main loop/frame
void CameraObj::tick() {
	if (permanentTarget) {
	  tracker->tick();
    follow(
      tracker->getX(),
      permanentTarget->getY(),
      tracker->getZ(),
      permanentTarget->getAngleY(),
      permanentTarget->getGrounded(),
      4
    );
  }
}

// Used to setup a target to always follow
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

float CameraObj::angleToGoal() {
  int deltaX = x-permanentTargetGoal->getX();
  int deltaZ = z-permanentTargetGoal->getZ();
  return deltasToDegrees(deltaX, deltaZ) + (deltaZ<0)*180;
}

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

// Do the following itself of your target
void CameraObj::follow(int a, int b, int c, int playerAngle, bool landed, int strictness) {

  // For smoothing purposes
  int num = 10;
  int den = 11; 
  int intendedDist = farthestDist;
  float newY = (b+lastLandedY)/2; // as to see jump height
  if (landed || lastLanded) {
    newY = b;
    lastLandedY = b;
  }
  lastLanded = landed; // because currently alternates between landed and not when actually landed

  // First, if following both, look between both, not at just one
  if (followingBoth) {
    // FIXME: For the presentation: getX/getZ aren't working but 0 is, so... whatever works for now
    a = (a*2 + (0 + a/2))/3;//(a + permanentTarget->getX()*3) / 4;
    c = (c*2 + (0 + c/2))/3;//(c + permanentTarget->getZ()*3) / 4;
    intendedDist = farthestDist * (goalRange + distToGoal()/10) / goalRange;
  }

  // Then start normal camera variables
  int distToPlayer = sqrt((x-a)*(x-a)+(z-c)*(z-c));

  // Extreme catchup for extreme dist scenario
  if (distToPlayer > 3*farthestDist) {
    x = (a+farthestDist*sin(angleY)*num + x)/den;
    z = (c+farthestDist*cos(angleY)*num + z)/den;
    y = ((camHeight*2-camHeight*distToPlayer/(farthestDist))*num + y)/den;
  }
  // so the camera doesn't go below a certain point
  int newDist = distToPlayer > farthestDist ? farthestDist : 
    (distToPlayer < closestDist ? closestDist : distToPlayer);
  y = ((lastLandedY+camHeight*2-camHeight*distToPlayer/(farthestDist)) + y*num)/den;
  float theAtan = deltasToDegrees(distToPlayer,y-newY);
  float angleXToBe =  theAtan != 0 ? 270 + theAtan : angleX;

  // The 1.0 is necessary for floating point division, as a/x/c/z are all ints
  //float angleYToBe = ((c-z<=0?0:180)+(((c!=z) ? atan((a-x)*1.0/(c-z)) : 3.14159/2)*360/(2*3.14159)));
  float angleYToBe = ((c-z<=0?0:180)+deltasToDegrees(a-x,c-z));
  // wanted else to = 360/4
  
  // if only following the player...
  if (!followingBoth) {
    // be inclined towards angle player is facing if following
    if ( withinRangeOf(tracker->getAngleY(),permanentTarget->getAngleY(),45) ) {
      playerAngle = matchRangeOf(playerAngle,angleYToBe);
      // Camera only turns if player going mostly away from camera
      if ((playerAngle < angleY + 125) && (playerAngle > angleY - 125)) {
        angleYToBe = (angleYToBe*num + playerAngle)/den;
      }
    }

    // But if within goal range, change to look at player AND the goal
    if (goalWithinDistRange() && goalWithinJumpRange()) {
      // Figure out which side to follow from
      cameraSide = findFollowingBothSide(angleYToBe, angleY);
      followingBoth = true;
    }

  // If you are following both the goal and the player
  } else {
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
  }

  //angleYToBe = matchRangeOf(angleYToBe, angleY);
  // A nice big buffer of 90 degrees makes this work where 1 degree didn't
  angleY = matchRangeOf(angleY, angleYToBe);
  //if (angleY < 0 && angleYToBe > 180) { angleY += 360; }
  //if (angleY > 180 && angleYToBe < 0) { angleY -= 360; }
  angleY += -(angleY-angleYToBe)/strictness;
  angleX += -(angleX-angleXToBe)/strictness;
  angleZ = 0; // Generally, don't want to change this - causes disorientation

  // Then implement movement for the camera in its new facing direction
  int viewingDist = (intendedDist + 3*distToPlayer)/4;
  int xToBe = a+viewingDist*sin(angleY*2*3.14159/360);
  int zToBe = c+viewingDist*cos(angleY*2*3.14159/360);
  x += -(x-xToBe)/strictness;
  z += -(z-zToBe)/strictness;
	
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

// Line of Sight - returns if view to player is clear or not
void CameraObj::setLOS(bool b) { los = b; }
bool CameraObj::getLOS() { return los; }
bool CameraObj::getVisibility() { return los; }
CubeObj* CameraObj::getPermanentTarget() { return permanentTarget; }
