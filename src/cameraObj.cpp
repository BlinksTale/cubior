/*
 * cameraObj.cpp
 * by Brian Handy
 * 3/29/12
 * Represents the game's camera location/etc.
 */

#include "cameraObj.h"
#include "gameplay.h"
#include <math.h>
#include <stdlib.h>

CameraObj::CameraObj() {
  // Pos vars
  x = 0;
  y = 200;
  z = -1000;
  angleX = 0;
  angleY = 0;
  angleZ = 0;
  
  // Follow vars
  farthestDist = 800;
}

void CameraObj::tick() {
  if (permanentTarget) {
    follow(permanentTarget->getX(),permanentTarget->getY(),permanentTarget->getZ());
  }
}

void CameraObj::alwaysFollow(CubeObj* target) {
  permanentTarget = target;
}

void CameraObj::follow(int a, int b, int c) {
  // For smoothing purposes
  int num = 29;
  int den = 30;
  int newY = b + 200; // as to see jump height

  int distToPlayer = sqrt((x-a)*(x-a)+(z-c)*(z-c));
if (distToPlayer > farthestDist) {
  x = (a+farthestDist*sin(angleY) + x*num)/den;
  z = (c+farthestDist*cos(angleY) + z*num)/den;
}
  y = ((1200-600*distToPlayer/(farthestDist)) + y*num)/den;
  angleX = (angleX*num+( y!=newY? 285+ atan(distToPlayer/(y-newY))*360/(2*3.14) : angleX))/den;
  int angleToBe = (c != z) ? ((c-z<0?0:180)+(atan((a-x)/(c-z))*360/(2*3.14159))) : angleY;//c-z > 0? 90 : 270;
  // Frankly, I don't understand it, but a nice big buffer of 90 degrees makes this work where 1 degree didn't
  if (angleY < 0 && angleToBe > 180) { angleY += 360; }
  if (angleY > 180 && angleToBe < 0) { angleY -= 360; }
  angleY =(angleY*num + angleToBe)/den;
  angleZ = 0; // Generally, don't want to change this one - it causes a disorienting effect
}

// Getters
int CameraObj::get(int s) { return s == 0 ? x : s == 1 ? y : z; }
int CameraObj::getX() { return x; }
int CameraObj::getY() { return y; }
int CameraObj::getZ() { return z; }
int CameraObj::getAngle(int s) { return s == 0 ? angleX : s == 1 ? angleY : angleZ; }
int CameraObj::getAngleX() { return angleX; }
int CameraObj::getAngleY() { return angleY; }
int CameraObj::getAngleZ() { return angleZ; }

// Set is absolute positioning
void CameraObj::setPos(int n, int o, int p) { x = n, y = o, z = p; }
void CameraObj::setX(int n) { x = n; }
void CameraObj::setY(int n) { y = n; }
void CameraObj::setZ(int n) { z = n; }
void CameraObj::setAngle(int n, int o, int p) { angleX = n, angleY = o, angleZ = p; }
void CameraObj::setAngleX(int n) { angleX = n; }
void CameraObj::setAngleY(int n) { angleY = n; }
void CameraObj::setAngleZ(int n) { angleZ = n; }
