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
}

void CameraObj::tick() {
  if (permanentTarget) {
    follow(
      permanentTarget->getX(),
      permanentTarget->getY(),
      permanentTarget->getZ(),
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
  follow(
      permanentTarget->getX(),
      permanentTarget->getY(),
      permanentTarget->getZ(),
      permanentTarget->getGrounded(),
      1
    );
}

void CameraObj::follow(int a, int b, int c, bool landed, int strictness) {
  // For smoothing purposes
  int num = 29;
  int den = 30;
  cout << "Landed is " << landed << " and lastLanded is " << lastLanded << endl;
  float newY = lastLandedY; // as to see jump height
  if (landed || lastLanded) {
    newY = b;
    lastLandedY = b;
  }
  lastLanded = landed; // because currently, cube alternates between landed and not when landed
  int distToPlayer = sqrt((x-a)*(x-a)+(z-c)*(z-c));

  // Extreme dist scenario
  if (distToPlayer > 3*farthestDist) {
    x = (a+farthestDist*sin(angleY)*num + x)/den;
    z = (c+farthestDist*cos(angleY)*num + z)/den;
    y = ((camHeight*2-camHeight*distToPlayer/(farthestDist))*num + y)/den;
  }
  // so the camera doesn't go below a certain point
  int distOrMaxDist = distToPlayer > 1.5*farthestDist ? 1.5*farthestDist : distToPlayer;
  y = ((camHeight*2-camHeight*distOrMaxDist/(farthestDist)) + y*num)/den;
  float theAtan = (y!=newY) ? atan(distToPlayer/((y-newY)*1.0))*360/(2*3.14159) : 0;
  float angleXToBe =  theAtan != 0 ? 270 + theAtan : angleX;
  // The 1.0 is necessary for floating point division, as a/x/c/z are all ints
  float angleYToBe = (c!=z) ? ((c-z<0?0:180)+(atan((a-x)*1.0/(c-z))*360/(2*3.14159))) : angleY;
  // A nice big buffer of 90 degrees makes this work where 1 degree didn't
  if (angleY < 0 && angleYToBe > 180) { angleY += 360; }
  if (angleY > 180 && angleYToBe < 0) { angleY -= 360; }
  angleX += -(angleX-angleXToBe)/strictness; //=(angleY*num + angleToBe)/den;
  angleY += -(angleY-angleYToBe)/strictness; //=(angleY*num + angleToBe)/den;
  angleZ = 0; // Generally, don't want to change this one - it causes a disorienting effect

  int xToBe = (a+farthestDist*sin(angleY*3.14159/360) + x*num)/den;
  int zToBe = (c+farthestDist*cos(angleY*3.14159/360) + z*num)/den;
  cout << "x-xToBe: " << x-xToBe << " and z-zToBe: " << z-zToBe << endl;
  cout << "Before x, z: (" << x << ", " << z << ")\n";
  x += -(x-xToBe)/strictness;
  z += -(z-zToBe)/strictness;

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
