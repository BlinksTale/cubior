/*
 * cubiorShape.cpp
 * by Brian Handy
 * 2/12/12
 * 3d cubior model
 */
#include "cubiorShape.h"
#include "gameplay.h"
#include "cubeShape.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <iostream>
using namespace std;

// Face smile height
static float faceSmileValue = 0.1;
static float faceSmileHeight = -0.2;
static float faceLineWidth = 0.1;
static float faceSmileWidth = 0.8;
static float faceSmileCurveWidth = 0.2;
static float faceSmileCurveHeight = 0.2;

void CubiorShape::initCubiorVisuals(int n) {
  srand(1);
  colorDarkness = 0.4;
  colorDefaultA = 0.4;
  switch(n) {
    case 0: // Yellow
      colorDefaultR = 0.2;
      colorDefaultG = 0.0;
      colorDefaultB =-0.4;
      break;
    case 1: // Blue
      colorDefaultR =-0.4;
      colorDefaultG = 0.0;
      colorDefaultB = 0.2;
      break;
    case 2: // Red
      colorDefaultR = 0.4;
      colorDefaultG =-0.2;
      colorDefaultB =-0.4;
      break;
    case 3: // Green
      colorDefaultR =-0.2;
      colorDefaultG = 0.4;
      colorDefaultB =-0.2;
      break;
    default:
      colorDefaultR =-0.4;
      colorDefaultG =-0.4;
      colorDefaultB =-0.4;
      break;
  }
  colorCurrentR = colorDefaultR;
  colorCurrentG = colorDefaultG;
  colorCurrentB = colorDefaultB;
  updateCubiorVisuals(n);
  initVisuals(r1,g1,b1,r1,g1,b1,colorDarkness,false,false);

  // fix default shadow status since player
  shadowState = true;
  directionalCulling = false;
}

void CubiorShape::updateCubiorVisuals(int n) {

  // Get colors in order
  updateCubiorColors(n);

  // Make sure smile lines up with Cubior's feelings
  faceSmileValue = (getPlayer(n)->getHappiness()-0.5)/10;

}

void CubiorShape::updateCubiorColors(int n) {

  // Invincible Colors
  if (getPlayer(n)->getInvincibility()) {
    colorCurrentR += (rand() % 4 - 2) / 4.0;
    colorCurrentG += (rand() % 4 - 2) / 4.0;
    colorCurrentB += (rand() % 4 - 2) / 4.0;
    if (colorCurrentR > 1.0) { colorCurrentR = 1.0; }
    if (colorCurrentG > 1.0) { colorCurrentG = 1.0; }
    if (colorCurrentB > 1.0) { colorCurrentB = 1.0; }
    if (colorCurrentR < colorDefaultA) { colorCurrentR = colorDefaultA; }
    if (colorCurrentG < colorDefaultA) { colorCurrentG = colorDefaultA; }
    if (colorCurrentB < colorDefaultA) { colorCurrentB = colorDefaultA; }
  } else {
  // Default Colors
    colorCurrentR = colorDefaultA;
    colorCurrentG = colorDefaultA;
    colorCurrentB = colorDefaultA;
  }

  if (!getPlayer(n)->getLock()) { colorCurrentR += colorDefaultR; colorCurrentG += colorDefaultG; colorCurrentB += colorDefaultB; }
  
  r1 = colorCurrentR + colorDarkness;
  g1 = colorCurrentG + colorDarkness;
  b1 = colorCurrentB + colorDarkness;
  r2 = colorCurrentR;
  g2 = colorCurrentG;
  b2 = colorCurrentB;
}

void CubiorShape::drawCubiorSilhouette() {
  glDisable(GL_DEPTH_TEST); // disable depth test for "shadow cubior"
  drawSilhouette();
  glEnable(GL_DEPTH_TEST); // Then return to normal stuff
}

void CubiorShape::drawCubior(int n){//float r1, float g1, float b1, float colorDarkness) {

  
  // make sure emotions are on the same page
  updateCubiorVisuals(n);

  // Cubior Matrix
  glPushMatrix();
  if (getPlayer(n)->getLock()) { glScalef(2.0,0.5,2.0); }

  bool hit = getPlayer(n)->getCollision();
  bool fly = getPlayer(n)->getNotGrounded();
  float gaspValue = 0.075;
  
  // call on cubeShape's function, drawCube, to make a cube visual
  draw();
  
  // Cubior Face Matrix
  glPushMatrix();
  glTranslatef(0.0,0.0,0.01);
  // Mouth
  if (!getPlayer(n)->getLock() && !hit) {
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2 + fly*gaspValue,0.5); // top left
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2 - fly*gaspValue,0.5); // bot left
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2,                       faceSmileHeight - faceLineWidth/2 + faceSmileValue,0.5); // bot right
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2,                       faceSmileHeight + faceLineWidth/2 + faceSmileValue,0.5); // top right
  glEnd();
  }
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2 + fly*gaspValue,0.5); // top left
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2 - fly*gaspValue,0.5); // bot left
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2 - fly*gaspValue,0.5); // bot right
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2 + fly*gaspValue,0.5); // top right
  glEnd();
  if (!getPlayer(n)->getLock() && !hit) {
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2,                       faceSmileHeight - faceLineWidth/2 + faceSmileValue,0.5); // bot left
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2 - fly*gaspValue,0.5); // bot right
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2 + fly*gaspValue,0.5); // top right
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2,                       faceSmileHeight + faceLineWidth/2 + faceSmileValue,0.5); // top left
  glEnd();
  }
  // Eye L
  if (hit) {
    drawClosedEyeAt( 0.2, 0.15, 0.5,false);
  } else {
    drawEyeAt( 0.2, 0.15, 0.5);
  }
  // Eye R
  if (hit) {
    drawClosedEyeAt(-0.2, 0.15, 0.5,true);
  } else {
    drawEyeAt(-0.2, 0.15, 0.5);
  }

  glPopMatrix();

  glPopMatrix();

}

void CubiorShape::drawEyeAt(float a, float b, float c) {
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(a-0.05,b+0.15,c+0.0);
    glColor3f(0,0,0); glVertex3f(a-0.05,b-0.15,c+0.0);
    glColor3f(0,0,0); glVertex3f(a+0.05,b-0.15,c+0.0);
    glColor3f(0,0,0); glVertex3f(a+0.05,b+0.15,c+0.0);
  glEnd();
}


void CubiorShape::drawClosedEyeAt(float a, float b, float c, bool flip) {
  float f = flip? 0.0 : 0.15;
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(a-0.10,b+0.15-f,c+0.0); // top right
    glColor3f(0,0,0); glVertex3f(a-0.00,b-0.00-f,c+0.0); // bot right
    glColor3f(0,0,0); glVertex3f(a+0.10,b-0.00-f,c+0.0); // bot left
    glColor3f(0,0,0); glVertex3f(a+0.00,b+0.15-f,c+0.0); // top left
  glEnd();
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(a-0.00,b+0.00+f,c+0.0); // top right
    glColor3f(0,0,0); glVertex3f(a-0.10,b-0.15+f,c+0.0); // bot right
    glColor3f(0,0,0); glVertex3f(a+0.00,b-0.15+f,c+0.0); // bot left
    glColor3f(0,0,0); glVertex3f(a+0.10,b+0.00+f,c+0.0); // top left
  glEnd();
}
