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
#include <iostream> // for cout

void GoalShape::initGoalVisuals() {
  colorDarkness = 0.4;
  colorDefaultA = 0.4;
  colorDefaultR = 0.6;
  colorDefaultG = 0.0;
  colorDefaultB = 0.0;
  colorCurrentR = colorDefaultR;
  colorCurrentG = colorDefaultG;
  colorCurrentB = colorDefaultB;
  glow = false;
  updateColors();
  initVisuals(r1,g1,b1,r1,g1,b1,colorDarkness,false,false);
  
  // fix default shadow status since goal
  defaultHasShadow = true;
}

void GoalShape::updateVisuals() {

  if (glow) {
    //std::cout << "increasing colors!" << std::endl;
    colorCurrentR += 0.005;
    colorCurrentG += 0.005;
    colorCurrentB += 0.005;
    if (colorDarkness > 0) {
      colorDarkness -= 0.0025;
    } else {
      colorDarkness = 0;
    }
  }
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
  r3 = colorCurrentR + colorDarkness;
  g3 = colorCurrentG + colorDarkness;
  b3 = colorCurrentB + colorDarkness;
}

void GoalShape::drawGoal(){//float r1, float g1, float b1, float colorDarkness) {

  // make sure emotions are on the same page
  updateVisuals();

  
  glDisable(GL_DEPTH_TEST); // disable depth test for "shadow goal"
  drawSilhouette();
  glEnable(GL_DEPTH_TEST); // Then return to normal stuff
  
  // call on cubeShape's function, drawCube, to make a cube visual
  draw();
}

bool GoalShape::getGlow() { return glow; }
void GoalShape::setGlow(bool b) { glow = b; }
