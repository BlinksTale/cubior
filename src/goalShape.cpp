/*
 * goalShape.cpp
 * by Brian Handy
 * 3/7/12
 * 3d goal model
 */
#include "cubiorShape.h"
#include "gameplay.h"
#include "goalShape.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>

void GoalShape::initGoalVisuals() {
  colorDarkness = 0.4;
  colorDefaultA = 0.4;
  colorDefaultR = 0.6;
  colorDefaultG = 0.0;
  colorDefaultB = 0.0;
  colorCurrentR = colorDefaultR;
  colorCurrentG = colorDefaultG;
  colorCurrentB = colorDefaultB;
  updateColors();
  initVisuals(r1,g1,b1,colorDarkness,false,false);
}

void GoalShape::updateVisuals() {

  // Get colors in order
  updateColors();
}

void GoalShape::updateColors() {
  r1 = colorCurrentR + colorDarkness;
  g1 = colorCurrentG + colorDarkness;
  b1 = colorCurrentB + colorDarkness;
  r2 = colorCurrentR;
  g2 = colorCurrentG;
  b2 = colorCurrentB;
}

void GoalShape::drawGoal(){//float r1, float g1, float b1, float colorDarkness) {

  // make sure emotions are on the same page
  updateVisuals();

  // call on cubeShape's function, drawCube, to make a cube visual
  draw();
}
