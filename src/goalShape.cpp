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
  shadowState = true;
  directionalCulling = false;
}

void GoalShape::updateVisuals() {

  if (glow) {
    //std::cout << "increasing colors!" << std::endl;
    colorCurrentR += 0.08;
    colorCurrentG += 0.08;
    colorCurrentB += 0.08;
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

    // Can just make a new array and transfer contents into myColors
  GLfloat newColors[] = { r1, g1, b1, // front top left
                        r1, g1, b1, // front top right
                        r2, g2, b2, // front bottom left
                        r2, g2, b2, // front bottom right
                        r1, g1, b1, // back top left
                        r1, g1, b1, // back top right
                        r2, g2, b2, // back bottom left
                        r2, g2, b2  // back bottom right
                      }; 

  for (int i=0; i<24; i++) {
    myColors[i] = newColors[i];
  }
  
  // And assign top colors
  GLfloat newTopColors[] = { r3, g3, b3, r3, g3, b3, r3, g3, b3, r3, g3, b3 }; // all top corners
  for (int i=0; i<12; i++) {
    topColors[i] = newTopColors[i];
  }

}

void GoalShape::drawGoal(){//float r1, float g1, float b1, float colorDarkness) {

  // make sure emotions are on the same page
  updateVisuals();

  
  glDisable(GL_DEPTH_TEST); // disable depth test for "shadow goal"
  drawSilhouette();
  glEnable(GL_DEPTH_TEST); // Then return to normal stuff
  
  // call on cubeShape's function, drawCube, to make a cube visual
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  draw();
  // deactivate vertex arrays after drawing
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}

bool GoalShape::getGlow() { return glow; }
void GoalShape::setGlow(bool b) { glow = b; }
