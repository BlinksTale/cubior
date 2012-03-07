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
  if (n == 0) {
    colorDefaultR = 0.2;
    colorDefaultG = 0.0;
    colorDefaultB =-0.4;
  } else {
    colorDefaultR =-0.4;
    colorDefaultG = 0.0;
    colorDefaultB = 0.2;
  }
  colorCurrentR = colorDefaultR;
  colorCurrentG = colorDefaultG;
  colorCurrentB = colorDefaultB;
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

void CubiorShape::drawCubior(int n){//float r1, float g1, float b1, float colorDarkness) {

  // make sure emotions are on the same page
  updateCubiorVisuals(n);

  // Cubior Matrix
  glPushMatrix();
  if (getPlayer(n)->getLock()) { glScalef(2.0,0.5,2.0); }

  // call on cubeShape's function, drawCube, to make a cube visual
  draw(r1,g1,b1,colorDarkness);
  

  // Cubior Face Matrix
  glPushMatrix();
  glTranslatef(0.0,0.0,0.01);
  // Mouth
  if (!getPlayer(n)->getLock()) {
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2,                       faceSmileHeight + faceLineWidth/2 + faceSmileValue,0.5); // top right
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2,                       faceSmileHeight - faceLineWidth/2 + faceSmileValue,0.5); // bot right
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2,0.5); // bot left
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2,0.5); // top left
  glEnd();
  }
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2,0.5); // top right
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2,0.5); // bot right
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2,0.5); // bot left
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2,0.5); // top left
  glEnd();
  if (!getPlayer(n)->getLock()) {
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2,                       faceSmileHeight + faceLineWidth/2 + faceSmileValue,0.5); // top left
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2,0.5); // top right
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2,0.5); // bot right
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2,                       faceSmileHeight - faceLineWidth/2 + faceSmileValue,0.5); // bot left
  glEnd();
  }
  // Eye L
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f( 0.25, 0.3,0.5);
    glColor3f(0,0,0); glVertex3f( 0.25, 0.0,0.5);
    glColor3f(0,0,0); glVertex3f( 0.15, 0.0,0.5);
    glColor3f(0,0,0); glVertex3f( 0.15, 0.3,0.5);
  glEnd();
  // Eye R
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(-0.25, 0.3,0.5);
    glColor3f(0,0,0); glVertex3f(-0.15, 0.3,0.5);
    glColor3f(0,0,0); glVertex3f(-0.15, 0.0,0.5);
    glColor3f(0,0,0); glVertex3f(-0.25, 0.0,0.5);
  glEnd();
  glPopMatrix();

  glPopMatrix();

}
