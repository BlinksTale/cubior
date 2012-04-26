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
  x = 0;
  y = 1111200;
  z = -1000;
  angleX = -45;
  angleY = 0;
  angleZ = 0;
  
  // Follow vars
  farthestDist = 800;
  closestDist = 600;
  idealDist = (farthestDist+closestDist)/2;
  tracker = new TrackerObj();
}

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

void CameraObj::alwaysFollow(CubeObj* target) {
  permanentTarget = target;
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

void CameraObj::follow(int a, int b, int c, int playerAngle, bool landed, int strictness) {
  // For smoothing purposes
  int num = 10;
  int den = 11; 
  float newY = (b + lastLandedY)/2; // as to see jump height
  if (landed || lastLanded) {
    newY = b;
    lastLandedY = b;
  }
  lastLanded = landed; // because currently alternates between landed and not when actually landed
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
  y = ((camHeight*2-camHeight*newDist/(farthestDist)) + y*num)/den;
  float theAtan = (y!=newY) ? atan(distToPlayer/((y-newY)*1.0))*360/(2*3.14159) : 0;
  float angleXToBe =  theAtan != 0 ? 270 + theAtan : angleX;

  // The 1.0 is necessary for floating point division, as a/x/c/z are all ints
  float angleYToBe = (c!=z) ? ((c-z<0?0:180)+(atan((a-x)*1.0/(c-z))*360/(2*3.14159))) : angleY;
  
  // be inclined towards angle player is facing if following
  if ( withinRangeOf(tracker->getAngleY(),permanentTarget->getAngleY(),45) ) {
    playerAngle = matchRangeOf(playerAngle,angleYToBe);
    angleYToBe = (angleYToBe*num + playerAngle)/den;
  }

  // A nice big buffer of 90 degrees makes this work where 1 degree didn't
  if (angleY < 0 && angleYToBe > 180) { angleY += 360; }
  if (angleY > 180 && angleYToBe < 0) { angleY -= 360; }
  angleY += -(angleY-angleYToBe)/strictness;
  angleX += -(angleX-angleXToBe)/strictness;
  angleZ = 0; // Generally, don't want to change this one - it causes a disorienting effect

  // Then implement movement for the camera in its new facing direction
  int xToBe = a+farthestDist*sin(angleY*2*3.14159/360);
  int zToBe = c+farthestDist*cos(angleY*2*3.14159/360);
  x += -(x-xToBe)/strictness;
  z += -(z-zToBe)/strictness;
}

// See if y1 within delta of y2
bool CameraObj::withinRangeOf(int y1, int y2, int delta) {
  matchRangeOf(y1,y2);
  return (y1 < y2+delta && y1 > y2-delta);
}

// Set y1 to within 180 of y2
int CameraObj::matchRangeOf(int y1, int y2) {
  if (y1 - y2 > 180) { y1 -= 360; }
  else if (y2 - y1 > 180) { y1 += 360; }
  return y1;
}

// Getters
int CameraObj::get(int s) { return s == 0 ? x : s == 1 ? y : z; }
int CameraObj::getX() { return x; }
int CameraObj::getY() { return y; }
int CameraObj::getZ() { return z; }
float CameraObj::getAngle(int s) { return s == 0 ? angleX : s == 1 ? angleY : angleZ; }
float CameraObj::getAngleX() { return angleX; }
float CameraObj::getAngleY() { return angleY; }
float CameraObj::getAngleZ() { return angleZ; }

// Set is absolute positioning
void CameraObj::setPos(int n, int o, int p) { x = n, y = o, z = p; }
void CameraObj::setX(int n) { x = n; }
void CameraObj::setY(int n) { y = n; }
void CameraObj::setZ(int n) { z = n; }
void CameraObj::setAngle(float n, float o, float p) { angleX = n, angleY = o, angleZ = p; }
void CameraObj::setAngleX(float n) { angleX = n; }
void CameraObj::setAngleY(float n) { angleY = n; }
void CameraObj::setAngleZ(float n) { angleZ = n; }
