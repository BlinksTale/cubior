/*
 * cameraObj.cpp
 * by Brian Handy
 * 3/29/12
 * Represents the game's camera location/etc.
 */

#include "cameraObj.h"
#include "cubeObj.h"
#include "cubiorObj.h"
#include "gameplay.h"
#include "trackerObj.h"

#define _USE_MATH_DEFINES
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
  
  // To cycle through camPos arrays
  currentCamSlot = 0;
    
  idealDist = (farthestDist+closestDist)/2;
  tracker = new TrackerObj();
  followingBoth = false; // Start by just following cube, not also goal
  nearGoal = false; // inherently won't start right under the goal
  los = true; // Assume you can see the player from the sky :P
}

// Set to starting pos, start of a new level
void CameraObj::resetPos() {
  if (permanentTarget) {
    x = permanentTarget->getX();
    y = permanentTarget->getY()+50*camHeight;
    z = permanentTarget->getZ()+6*tileSize;//getMapDepth()*tileSize/2;//4*camHeight;
  } else {
    x = 0;
    y = 1000000;
    //z = -1000;
    //x = 0;
    z = getMapDepth()*tileSize/2+tileSize*6;
  }
  angleX = 0;
  angleY = 0;
  angleZ = 0;
  lastAngleY = 0.0;
  lastAngleDelta = 0.0;
  
  wallState = false;
  intendedState = false;
  goalState = false;
  freeState = true;
  
  // start with no intended pos and full movement freedom
  foundIntendedPos = false;
  justFixedVisibility = false;
  freedom = true;
  visibleIntendedCount = 0;
  backupFreedom = true;
  lastDistToIntended = 0;
  intendedStuckCount = 0;
  lockedToPlayer = false;
  lockedToPlayerX = false;
  lockedToPlayerZ = false;
  
  //cout << "resetting array" << endl;
  for (int i=0; i<camArraySize; i++) {
    updateCamArray();
  }
  updateMeans();
}

// Used for smoothing, avgs last camArraySize pos and angles
void CameraObj::updateCamArray() {
  camPosX[currentCamSlot] = x;
  camPosY[currentCamSlot] = y;
  camPosZ[currentCamSlot] = z;
  /*while (angleX > 180+meanAngleX) { angleX -= 360; }
  while (angleX <-180+meanAngleX) { angleX += 360; }
  while (angleY > 180+meanAngleY) { angleY -= 360; }
  while (angleY <-180+meanAngleY) { angleY += 360; }
  while (angleZ > 180+meanAngleZ) { angleZ -= 360; }
  while (angleZ <-180+meanAngleZ) { angleZ += 360; }
  */
  // FIXME: This still feels really laggy, can't it be more efficient? Maybe never matchrangeof?
  for (int i=0; i<camArraySize; i++) {
    //if (outsideRangeOf(meanAngleX,angleX)) { 
    camAngleX[i] = matchRangeOf(camAngleX[i], angleX);
    //}
    //if (outsideRangeOf(meanAngleY,angleY)) { 
    camAngleY[i] = matchRangeOf(camAngleY[i], angleY);
    //}
    //if (outsideRangeOf(meanAngleZ,angleZ)) { 
    camAngleZ[i] = matchRangeOf(camAngleZ[i], angleZ);
    //}
  }
  camAngleX[currentCamSlot] = angleX;
  camAngleY[currentCamSlot] = angleY;
  camAngleZ[currentCamSlot] = angleZ;
  currentCamSlot = (currentCamSlot + 1) % camArraySize;
}

void CameraObj::updateMeans() {
  // Pos
  meanX = 0;
  meanY = 0;
  meanZ = 0;
  // Angles
  meanAngleX = 0.0;
  meanAngleY = 0.0;
  meanAngleZ = 0.0;
  // Fill 'em
  for (int i=0; i<camArraySize; i++) {
    meanX += camPosX[i];
    meanY += camPosY[i];
    meanZ += camPosZ[i];
    //cout << "Old meanAngleX " << meanAngleX << " vs camAngleX[" << i << "] " << camAngleX[i] << endl;
   // meanAngleX = matchRangeOf(camAngleX[i],meanAngleX);
    meanAngleX += camAngleX[i];
    meanAngleY += camAngleY[i];
    meanAngleZ += camAngleZ[i];
  }
  // And divide 'em back down to size
  meanX /= camArraySize;
  meanY /= camArraySize;
  //meanY = -2400; // DELEMETE angleX problem
  meanZ /= camArraySize;
  meanAngleX /= camArraySize;
  meanAngleY /= camArraySize;
  meanAngleZ /= camArraySize;
  //cout << "MeanAngleX was " << meanAngleX << endl;
  //cout << "meanAngleX is " << meanAngleX << endl;
  /*while (meanAngleX < 0)   { meanAngleX += 360; }
  while (meanAngleX > 360) { meanAngleX -= 360; }
  if (meanAngleX < 180) {
    if (meanAngleX > 90) { meanAngleX = 90; } 
    if (meanAngleX < 0)  { meanAngleX = 0;  }
  } else {
    if (meanAngleX > 360) { meanAngleX = 360; }
    if (meanAngleX < 270) { meanAngleX = 270; }
  }*/
  //meanAngleX = 370;
  //cout << "MeanAngleX now is " << meanAngleX << endl;
}

void CameraObj::updateJustFixedVisibility() {
  if (justFixedVisibility && justFixedMaxDist <
        sqrt((double)(pow((double)(justFixedX-permanentTarget->getX()),(double)(2)) + pow((double)(justFixedZ-permanentTarget->getZ()),(double)(2))))) {
  justFixedVisibility = false;
  //cout << "disable, justFixedVisibility = " << justFixedVisibility << endl;
  }
}

// To use first camera style, nothing else must be required of it
bool CameraObj::freeMovementState() {
  return freedom && !foundIntendedPos && playerRegularlyVisible(permanentTargetNum) &&
      ((!goalOutsideDistRange() && goalWithinJumpRange()) ||
       (!lockedToPlayer && !lockedToPlayerX && !lockedToPlayerZ));
}

// First camera movement style, freely follow player
// by casually catching up to them
void CameraObj::applyFreeMovement() {
  if (showData) { cout << "Option One" << endl; }
      
  // As long as angle was not just recently modified, follow normally
  if (!justFixedVisibility) {
    //cout << "Normal loop! Freedom found, no intended pos" << endl;
    follow(
      tracker->getX(),
      permanentTarget->getY(),
      tracker->getZ(),
      permanentTarget->getAngleY(),
      permanentTarget->getGrounded(),
      4
    );
  } else {
    // If you just fixed visibility, don't mess it up.
    // Look at the player, but don't try changing directions
    // FIXME: Odd, but this doesn't work! Camera is looking... elsewhere?
    lookAtPlayer(
      permanentTarget->getX(), // was tracker, changed since trying desperately to look at cubior
      permanentTarget->getY(),
      permanentTarget->getZ(), // was tracker, changed since trying desperately to look at cubior
      permanentTarget->getAngleY(),
      permanentTarget->getGrounded(),
      4);
  }
}

// Second camera movement style, locks its position
// in relation to the player
void CameraObj::applyLockedToPlayer() {
  if (showData) { cout << "Option Two" << endl; }
  x = tracker->getX() + lockedX;
  y = tracker->getY() + lockedY;
  z = tracker->getZ() + lockedZ;
  angleY = lockedAngleY;
  // Must snap to 45 degree intervals
  if (!getSnapLock()) { snapLock(); }
  // And be within range of player
  if (groundDistToPlayer() > farthestDist) {
    changePosTowards(
            permanentTarget,
            camSpeed); }
}

// Third camera movement style, locks its position
// in relation to the player's X axis alone
void CameraObj::applyLockedToPlayerX() {
  if (showData) { cout << "Option Three" << endl; }
  x = tracker->getX() + lockedX;
  y = tracker->getY() + lockedY;
  z = tracker->getZ() + lockedZ;
  angleY = lockedAngleY;
  // Must snap to 45 degree intervals
  if (!getSnapLock()) { snapLock(); }
  // And be within range of player
  if (groundDistToPlayer() > farthestDist) { changePosTowards(permanentTarget,camSpeed); }
}

// Fourth camera movement style, locks its position
// in relation to the player's Z axis alone
void CameraObj::applyLockedToPlayerZ() {
  if (showData) { cout << "Option Four" << endl; }
  x = tracker->getX() + lockedX;
  y = tracker->getY() + lockedY;
  z = tracker->getZ() + lockedZ;
  angleY = lockedAngleY;
  // Must snap to 45 degree intervals
  if (!getSnapLock()) { snapLock(); }
  // And be within range of player
  if (groundDistToPlayer() > farthestDist) {
    changePosTowards(permanentTarget,camSpeed);
  }
}

// Gives whether angle is at a 45 degree interval
bool CameraObj::getSnapLock() {
  //cout << "AND ANGLEY IS " << angleY << endl;
  while (lockedAngleY < 0) { lockedAngleY += 360; }
  int range = 1;
  bool result = ((int)(lockedAngleY) % 45 < range) || ((int)(lockedAngleY) % 45 > 45 - range);
  //cout << "SNAP LOCKED IS " << result << " FOR " << lockedAngleY << endl;
  return result;
}

// Snap lockedAngleY into a 45 degree increment
void CameraObj::snapLock() {
  while (lockedAngleY < 0) { lockedAngleY += 360; }
  int distFromLock = (int)(lockedAngleY) % 45;
  int extraOne = (distFromLock > 22 ? 1 : 0);
  //cout << "DIST FROM LOCK " << distFromLock << " SO EXTRA ONE IS " << extraOne << endl;
  lockedAngleY = (floor((lockedAngleY - distFromLock) / 45.0) + extraOne) * 45.0;

  // Then update lockedX, Y and Z

}

float CameraObj::getLockedAngleY() { return lockedAngleY; }

// Fifth camera movement style moves towards some exact
// intendedPos, stopping and confirming justFixed upon arrival
void CameraObj::applyIntendedPos() {
  if (showData) { cout << "Option Five" << endl; }
  justFixedVisibility = true;
  //cout << "You have no freedom (" << (!freedom) << ") or have found an intended position (" << foundIntendedPos << ")!" << endl;
  // if you do have a place to be or aren't allowed to move,
  if (foundIntendedPos) {
    //cout << "w/ intended pos found" << endl;
    // Make sure camera doesn't swing back and forth
    //float currentAngleDelta = angleY - lastAngleY;
    //lastAngleY = angleY;

    if (showData) { 
    cout << "Intended Position found, dist to cube is " << distToCube(&intendedPos) << endl;
    cout << "so intendedPos " << intendedPos.getX() << ", " << intendedPos.getY() << ", " << intendedPos.getZ() << endl;
    cout << "&  currentPos  " << x << ", " << y << ", " << z << endl;
    cout << "so it being greater than camSpeed of " << camSpeed << " is " << (distToCube(&intendedPos) > camSpeed) << endl;
    //cout << "and currentAngleDelta " << currentAngleDelta << " times lastAngleDelta " << lastAngleDelta << " must be >=0." << endl;
    }
        
        
    // If still not at the destination, and have not changed direction of movement
    if (distToCube(&intendedPos) > camSpeed) {/* &&
            (currentAngleDelta >= 0 && lastAngleDelta >= -1) ||
            (currentAngleDelta <= 0 && lastAngleDelta <= 1)) {
      lastAngleDelta = currentAngleDelta;*/
          
      if (showData) { 
        cout << "Moving towards intended pos" << endl;
        cout << "currentPos  " << x << ", " << y << ", " << z << endl;
      }
      // Move towards it
      changePosTowards(&intendedPos,camSpeed);
      if (showData) { 
        cout << "Move success!" << endl;
        cout << "currentPos  " << x << ", " << y << ", " << z << endl;
      }
          
      //cout << "last" << lastDistToIntended << " - dist " << distToCube(&intendedPos) << endl;
      // If you get stuck trying to get to intended
      if (abs(lastDistToIntended - distToCube(&intendedPos))<camSpeed) {
        //cout << "Fire1" << endl;
        // keep track of how long you've been stuck
        intendedStuckCount++;
        // if this is too long a stick, just jump!
        if (intendedStuckCount >= intendedStuckMax) {
          //cout << "Fire1a" << endl;
          setPos(intendedPos.getX(),intendedPos.getY(),intendedPos.getZ());
          intendedStuckCount = 0; // and reset stuck count, of course
        }
      } else {
        if (intendedStuckCount > 0) {
          //cout << "FREE AGAIN" << endl;
          intendedStuckCount = 0; // no stick? Reset stuck count
        }
        // if you finally got free, update the lastDistToIntended
        // finally, update how far you are from player
        lastDistToIntended = distToCube(&intendedPos);
      }
    } else {
      //cout << "No need to move, already here" << endl;
      if (showData) { cout << "currentPos1 " << x << ", " << y << ", " << z << endl; }
      x = intendedPos.getX();
      y = intendedPos.getY();
      z = intendedPos.getZ();
      if (showData) {
        cout << "JUMP TO POS" << endl;
        cout << "currentPos2 " << x << ", " << y << ", " << z << endl; 
      }
      // You're there! Stop trying.
      foundIntendedPos = false;
      justFixedVisibility = true;
      justFixedX = permanentTarget->getX();
      justFixedZ = permanentTarget->getZ();
      lastAngleDelta = 0;
      // currently disabled to try and find a technique without this
      //freedom = false;
      if (showData) { cout << "intendedPos and freedom set to false" << endl; }
    }
  } else {
    // No intended Pos, but need one? Call on gameplay to do it!
    // This is key, apparently! Works well now though :)
    rotateToPlayer(permanentTargetNum,0);
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
  if (showData) { 
    cout << "intendedPos " << intendedPos.getX() << ", " << intendedPos.getY() << ", " << intendedPos.getZ() << endl;
    cout << "currentPos  " << x << ", " << y << ", " << z << endl;
    cout << "targetPos   " << permanentTarget->getX() << ", " << permanentTarget->getY() << ", " << permanentTarget->getZ() << endl;
  }
  // FIXME: Pretty darn sure this whole chunk I just added must be broken. groundDistToPlayer()? Really?
  /*positionByAngles(
    tracker->getX(),
    tracker->getZ(),
    100,
    groundDistToPlayer(),
    permanentTarget->getAngleY(),
    4);*/
}

// The most basic increment, called once per main loop/frame
void CameraObj::tick() {
  //cout << "Start of tick,"<< endl;
  //cout << "starting currentPos  " << x << ", " << y << ", " << z << endl;

  // If you are following one target every frame,
	if (permanentTarget) {
    // make sure to update the item that's following the target
	  tracker->tick();
    
    // And give freedom back if player moved away from just fixed location
    updateJustFixedVisibility();
    
    // then move the camera itself if free to do so
    if (freeMovementState()) {
      applyFreeMovement();
    // Locked to player then? Keep up with them!
    } else if (lockedToPlayer) {
      applyLockedToPlayer();
    // Locked to player X then? Keep up with them!
    } else if (lockedToPlayerX) {
      applyLockedToPlayerX();
    // Locked to player Z then? Keep up with them!
    } else if (lockedToPlayerZ) {
      applyLockedToPlayerZ();
    } else { // not free or have an intended pos
      applyIntendedPos();
    } // END OF OPTION FIVE


    // Ultimatum Look at player no matter what
    // Probably don't want to do this, but seeing if it fixes second green level problem
    // was Tracker for X/Z, didn't work, trying permanentTarget now
    // Still no luck! What is going on?
    /*lookAtPlayer(
    permanentTarget->getX(),
    permanentTarget->getY(),
    permanentTarget->getZ(),
    permanentTarget->getAngleY(),
    permanentTarget->getGrounded(),
    4);*/

  }
  //cout << "End camera tick" << endl;
  updateCamArray();
  updateMeans();
}

// Used to permanently set a target to always follow
void CameraObj::alwaysFollow(CubeObj* target, CubeObj* targetGoal, int targetNum) {
  permanentTarget = target;
  permanentTargetNum = targetNum;
  permanentTargetGoal = targetGoal;
  lastLandedY = target->getY();
  lastLanded = target->getLanded();
  //x = target->getX()-camHeight;
  //y = target->getY()+50*camHeight;
  //z = target->getZ()-6*tileSize;//4*camHeight;
  tracker->setTarget(permanentTarget);
  resetPos();
  follow(
      tracker->getX(),
      permanentTarget->getY(),
      tracker->getZ(),
      permanentTarget->getAngleY(),
      permanentTarget->getGrounded(),
      1
    );
}

// Farthest/Max dist we want to be from player
int CameraObj::getFarthestDist() {
  return farthestDist;
}

// Find how close player is to their goal,
// if close enough we'll use this to look at both at once
int CameraObj::distToGoal() {
  int deltaX = permanentTarget->getX()-permanentTargetGoal->getX();
  int deltaZ = permanentTarget->getZ()-permanentTargetGoal->getZ();
  return sqrt((double)(deltaX*deltaX + deltaZ*deltaZ));
}

// Find how close player is to camera
int CameraObj::distToPlayer() {
  int deltaX = x-permanentTargetGoal->getX();
  int deltaY = y-permanentTargetGoal->getY();
  int deltaZ = z-permanentTargetGoal->getZ();
  return sqrt((double)(sqrt((double)(deltaX*deltaX + deltaZ*deltaZ))*sqrt((double)(deltaX*deltaX + deltaZ*deltaZ))+deltaY*deltaY));
}

// Find how high player is to camera
int CameraObj::heightToPlayer() {
  int deltaY = y-permanentTargetGoal->getY();
  return abs(deltaY);
}

// Find how close player is to camera on X,Z
int CameraObj::groundDistToPlayer() {
  //cout << "Get groundDistToPlayer for " << (permanentTarget->getX()) << ", " << (permanentTarget->getZ()) << endl;
  return groundDistTo(permanentTarget->getX(),permanentTarget->getZ());
}

// Find how close camera is to some X,Z
int CameraObj::groundDistTo(int a, int c) {
  //cout << "So for x,z as " << x << ", " << z << endl;
  //cout << "and a,c as " << a << ", " << c << endl;
  int deltaX = x-a;
  int deltaZ = z-c;
  //cout << "We have deltaX " << deltaX << " and deltaZ " << deltaZ << endl;
  int result = sqrt((double)(deltaX*deltaX + deltaZ*deltaZ));
  //cout << "With result " << result << endl;
  return result;
}

// Ground angle to goal from camera
float CameraObj::angleToGoal() {
  int deltaX = x-permanentTargetGoal->getX();
  int deltaZ = z-permanentTargetGoal->getZ();
  return deltasToDegrees(deltaX, deltaZ) + (deltaZ<0)*180;
}

// Ground angle to goal from player/permanentTarget
float CameraObj::angleBetweenPlayerAndGoal() {
  // NEW WAY that never really got to be working
  float result = getAngleBetween(permanentTargetGoal->getX(),permanentTargetGoal->getZ(),permanentTarget->getX(),permanentTarget->getZ())*360/(2*M_PI);
  result -= 90;
  result = -result;
  // NEWDELETEME: cout << "RESULT IS " << result << endl;
  //return result;*/
  int deltaX = permanentTarget->getX()-permanentTargetGoal->getX();
  int deltaZ = permanentTarget->getZ()-permanentTargetGoal->getZ();
  float oldResult = deltasToDegrees(deltaX, deltaZ) + (deltaZ<0)*180;
  // NEWDELETEME: cout << "OLDULT IS " << oldResult << endl;
  // NEWDELETEME: cout << "GETCAM IS " << getAngleY() << endl;
  return result;//oldResult;
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
  float result;
  
  if (adj != 0) {
    result = atan(opp*1.0/(adj*1.0));
  } else {
    result = (-2*(opp > 0)+1)*M_PI/2.0;
  }

  // This accounts for how atan only covers the top half of the circle for degrees
  // cout << "result at this point is " << result << endl;

  // This code was supposed to fix atan's wraparound issues... ended up causing them?
  // Commented out and saved just in case things go horribly wrong somehow.
  //if (opp < 0) { result += (-2*(adj<0)+1)*PI-result; } // true = 1, false = 0

  // Then convert radians to degrees
  result *= 360.0/(2.0*M_PI);
  return result;
}

float CameraObj::fixedDeltasToDegrees(int opp, int adj) {
  float result;
  
  if (adj != 0) {
    result = atan(opp*1.0/(adj*1.0));
  } else {
    // Just straight up look out (assuming fixed is only used for camera)
    // with a 90 degree return value.
    result = M_PI/2.0 - M_PI*(opp<0);
    //(-2*(opp > 0)+1)*M_PI/2.0;
  }

  // If adj < 0, must flip it over to be above 90 degree mark and negated
  // to move from 90 to 180 instead of from 0 to -90.
  if (adj < 0) {
    result = -result + M_PI/2.0;
  }

  // Then convert radians to degrees
  result *= 360.0/(2.0*M_PI);
  return result;
}


// When camera starts following both, decide from which angle
float CameraObj::findFollowingBothSide(float angleYToBe, float angleY) {
  float atg = angleBetweenPlayerAndGoal();//angleToGoal();
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
  // NEWDELETEME: cout << "YEP, HERE WITH ANGLEY = " << angleY << endl;
  positionByAngles(a,c,intendedDist,distToPlayer,angleY,strictness);
}

// position by angles with no changes to anything current
void CameraObj::maintainPosition() {
  int a = tracker->getX();
  int c = tracker->getZ();
  int distToPlayer = groundDistTo(a,c);
  int intendedDist = findIntendedDist(a,c);
  positionByAngles(a,c,intendedDist,distToPlayer,angleY,1);
}

void CameraObj::lookAtTarget() {
  lookAtPlayer(
        tracker->getX(),
        permanentTarget->getY(),
        tracker->getZ(),
        permanentTarget->getAngleY(),
        permanentTarget->getGrounded(),
        4);
}
// Bare basics of any normal tick/update: look at the player
void CameraObj::lookAtPlayer(int a, int b, int c, int playerAngle, bool landed, int strictness) {
    int num = 10, den = 11; 
    /* Vertical aiming segment */
    float targetY = findTargetY(b, landed);
    // quick temp var for the triangle between ground hyp and y diff
    //cout << "groundDistTo(" << a << "," << c <<") = " << groundDistTo(a,c) << endl;
    //cout << "y " << y << " - TargetY " << targetY << " = " << y-targetY << endl;
    float vertAtan = fixedDeltasToDegrees(groundDistTo(a,c),y-targetY);  
    //cout << "vertAtan = " << vertAtan << endl;
    // angleXToBe is how far down to be looking, from the camera, to the player
    float angleXToBe = 270 + vertAtan;// vertAtan != 0 ? 270 + vertAtan : angleX;
    //if (angleXToBe > 360) { y++; }
    //cout << "xToBe " << angleXToBe << endl;

    /* Horizontal aiming segment */
    // The 1.0 is necessary for floating point division, as a/x/c/z are all ints
    float angleYToBe = ((c-z<=0?0:180)+deltasToDegrees(a-x,c-z));

    // Figure out follow-just-the-player mode
    // can do this if you just fixed visibility too
    if (!followingBoth || justFixedVisibility) {
      // Only point the way the player faces if you're moving
      //cout << "momentum x/z > 10 = " << (abs(permanentTarget->getMomentumX())>10 || abs(permanentTarget->getMomentumZ())>10) << endl;
      if (abs(permanentTarget->getMomentumX())>10 || abs(permanentTarget->getMomentumZ())>10) {
        angleYToBe = followOne(angleYToBe, playerAngle, num, den);
        backupFreedom = true; // if player's moving again, feel the freedom to backup
      }
    // Otherwise, watch the player *and* the goal
    } else {
      angleYToBe = followBoth(angleYToBe);
    }

    // Then update the final angles themselves
    angleY = matchRangeOf(angleY, angleYToBe);
    angleY += -(angleY-angleYToBe)/strictness;
    // NEWDELETEME: cout << "ANGLEY IS " << angleY << endl;
    angleX += -(angleX-angleXToBe)/strictness;
    angleZ = 0; // Generally, don't want to change this - causes disorientation
}

// Final step of follow, adjust position to match camera angles
void CameraObj::positionByAngles(int a, int c, int intendedDist, int distToPlayer, float angleY, int strictness) {
  int viewingDist = (intendedDist + 3*distToPlayer)/4;
  // NEWDELETEME: cout << "NOW ANGLEY IS " << angleY << " FOR VIEWINGDIST " << viewingDist << endl;
  // NEWDELETEME: cout << "SO ANGLEY*2*M_PI/360 = " << angleY*2*M_PI/360.0 << endl;
  // NEWDELETEME: cout << "GIVING SIN " << sin(angleY*2*M_PI/360.0) << " AND COS " << cos(angleY*2*M_PI/360.0) << endl;
  int xToBe = a+viewingDist*sin(angleY*2*M_PI/360.0);
  int zToBe = c+viewingDist*cos(angleY*2*M_PI/360.0);
  // NEWDELETEME: cout << "GIVING US " << xToBe << " BY " << zToBe << " FROM " << a << " BY " << c << endl;

  // This will be a bit tricky, need to determine if new dist is smaller than old
  // when backup freedom is disabled
  int newDist = sqrt(pow((double)(zToBe-permanentTarget->getZ()),(double)(2))+pow((double)(xToBe-permanentTarget->getX()),(double)(2)));
  int oldDist = sqrt(pow((double)(z-permanentTarget->getZ()),(double)(2))+pow((double)(x-permanentTarget->getX()),(double)(2)));
  bool newDistSmaller = newDist < oldDist;
  
  if (backupFreedom || newDistSmaller) {
    x += -(x-xToBe)/strictness;
    z += -(z-zToBe)/strictness;
  }
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
  // however, not if already locked to an axis
  if (goalWithinDistRange() && goalWithinJumpRange()
    && !lockedToPlayer && !lockedToPlayerX && !lockedToPlayerZ) {
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

bool CameraObj::outsideRangeOf(int y1, int y2) {
  return !withinRangeOf(y1, y2, 360);
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

// Regarding place we want camera to go in order to see player again
void CameraObj::setIntendedPos(CubeObj* c) {
  intendedPos.setPos(c->getX(),c->getY(),c->getZ());
  foundIntendedPos = true;
}
bool CameraObj::getFoundIntendedPos() { return foundIntendedPos; }
void CameraObj::setFoundIntendedPos(bool b) { foundIntendedPos = b; }
void CameraObj::disableIntendedPos() { foundIntendedPos = false; }
// And if we can see the player early, keep tally of how long
int CameraObj::getVisibleIntended() { return visibleIntendedCount; }
void CameraObj::setVisibleIntended(int i) { visibleIntendedCount = i; }

// Backup Freedom stuff, for when headed towards intended pos
bool CameraObj::getBackupFreedom() { return backupFreedom; }
void CameraObj::setBackupFreedom(bool b) { backupFreedom = b; }

/***********/
/* Getters */
/***********/

// Straight pos
int CameraObj::get(int s) { return s == 0 ? x : s == 1 ? y : z; }
int CameraObj::getX() { return x; }
int CameraObj::getY() { return y; }
int CameraObj::getZ() { return z; }

// Average pos of the last camArraySize positions
int CameraObj::getMean(int s) {  
  return s==0? meanX : s==1? meanY : meanZ;
}
int CameraObj::getMeanX() { return getMean(0); }
int CameraObj::getMeanY() { return getMean(1); }
int CameraObj::getMeanZ() { return getMean(2); }

// Straight angle
float CameraObj::getAngle(int s) {
  return s == 0 ? angleX : s == 1 ? angleY : angleZ;
}
float CameraObj::getAngleX() { return angleX; }
float CameraObj::getAngleY() { return angleY; }
float CameraObj::getAngleZ() { return angleZ; }
float CameraObj::getRadiansAngleY() { return ((270-(int)angleY)%360)*2.0*M_PI/360.0; }

// Average angle of the last camArraySize positions
float CameraObj::getMeanAngle(int s) {
  return s==0? meanAngleX : s==1? meanAngleY : meanAngleZ;
}
float CameraObj::getMeanAngleX() { return getMeanAngle(0); }
float CameraObj::getMeanAngleY() { return getMeanAngle(1); }
float CameraObj::getMeanAngleZ() { return getMeanAngle(2); }

/***********/
/* Setters */
/***********/

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

bool CameraObj::getLockedToPlayer() { return lockedToPlayer; }
bool CameraObj::getLockedToPlayerX() { return lockedToPlayerX; }
bool CameraObj::getLockedToPlayerZ() { return lockedToPlayerZ; }
void CameraObj::setLockedToPlayer(bool b) { 
  resetLocks();
  lockedToPlayer = b;
}
void CameraObj::setLockedToPlayerX(bool b) { 
  resetLocks();
  lockedToPlayerX = b;
}
void CameraObj::setLockedToPlayerZ(bool b) { 
  resetLocks();
  lockedToPlayerZ = b;
}
// For any type of locking
void CameraObj::resetLocks() {
  lockedX = x - permanentTarget->getX();
  lockedY = y - permanentTarget->getY();
  lockedZ = z - permanentTarget->getZ();
  lockedY = lockedY < camHeight ? camHeight : lockedY;
  lockedAngleY = angleY;
  snapLock();
}

// Give the camera's current state,
// what it wants to do most of all.
int CameraObj::getState() {
  // This is also the priority order
  if (goalState) { return 1; } // goal is most important
  if (wallState) { return 3; } // second is walls
  if (intendedState) { return 2; } // third visibility
  if (freeState) { return 0; } // finally freedom
}
// Force cam to change states
void CameraObj::setState(int i) {
  goalState = false;
  wallState = false;
  intendedState = false;
  freeState = false;
  switch (i) {
    case 1: goalState = 1; break;
    case 3: wallState = 1; break;
    case 2: intendedState = 1; break;
    default: freeState = 1; break;
  }
}