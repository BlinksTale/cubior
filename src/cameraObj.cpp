/*
 * cameraObj.cpp
 * by Brian Handy
 * 3/29/12
 * Represents the game's camera location/etc.
 */

#include "cameraObj.h"
#include "gameplay.h"

CameraObj::CameraObj() {
  // Pos vars
  x = 0;
  y = -200;
  z = 1000;
  angleX = 0;
  angleY = 0;
  angleZ = 0;
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
  x = a + 1000;
  y = b + 200;
  z = c + 1000;
  angleX = 0;
  angleY = 45;
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
