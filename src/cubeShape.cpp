/*
 * cubeShape.cpp
 * by Brian Handy
 * 2/2/12
 * 3d cube model
 */
#include "cubeShape.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void CubeShape::initCubeVisuals() {
  // Nothing here yet! Colors are what distinguish Cubiors from Cubes atm
}

void CubeShape::updateCubeVisuals() {
  // This handles colors and face smile for Cubiors, but nothing for Cubes yet
}

void CubeShape::drawCube(float colorCurrentR, float colorCurrentG, float colorCurrentB, float colorDarkness) {

  // make sure visuals are on the same page
  updateCubeVisuals();

  float r1 = colorCurrentR;
  float g1 = colorCurrentG;
  float b1 = colorCurrentB;
  float r2 = colorCurrentR - colorDarkness;
  float g2 = colorCurrentG - colorDarkness;
  float b2 = colorCurrentB - colorDarkness;

// Draw Cubior, the cube!
    // Back
    glBegin(GL_POLYGON);
    glColor3f(r1-0.2,g1-0.2,b1-0.2); glVertex3f( 0.5, 0.5,-0.5);
    glColor3f(r1-0.2,g1-0.2,b1-0.2); glVertex3f(-0.5, 0.5,-0.5);
    glColor3f(r2-0.2,g2-0.2,b2-0.2); glVertex3f(-0.5,-0.5,-0.5);
    glColor3f(r2-0.2,g2-0.2,b2-0.2); glVertex3f( 0.5,-0.5,-0.5);
    glEnd();
    // Left
    glBegin(GL_POLYGON);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5,-0.5);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5, 0.5);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5, 0.5);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5,-0.5);
    glEnd();
    // Right
    glBegin(GL_POLYGON);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5,-0.5);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5, 0.5);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5, 0.5);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5,-0.5);
    glEnd();
    // Bottom
    glBegin(GL_POLYGON);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5, 0.5);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5, 0.5);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5,-0.5);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5,-0.5);
    glEnd();
    // Top
    glBegin(GL_POLYGON);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5,-0.5);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5, 0.5);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5, 0.5);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5,-0.5);
    glEnd();
    // Front
    glBegin(GL_POLYGON);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5,0.5);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5,0.5);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5,0.5);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5,0.5);
    glEnd();
}
