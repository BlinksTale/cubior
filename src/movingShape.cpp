//
//  movingShape.cpp
//  Cubior
//
//  Created by Brian Handy on 7/13/14.
//
//

#include "movingShape.h"
#include "cubiorShape.h"
#include "movingObj.h"
#include "cubeObj.h"
#include "gameplay.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <iostream> // for cout
#define _USE_MATH_DEFINES
#include <math.h>

void MovingShape::initVisuals() {
  colorDarkness = 0.4;
  colorDefaultA = 0.4;
  colorDefaultR = 0.0;
  colorDefaultG = 0.0;
  colorDefaultB = 0.6;
  colorCurrentR = colorDefaultR;
  colorCurrentG = colorDefaultG;
  colorCurrentB = colorDefaultB;
  updateColors();
  super::initVisuals(r1,g1,b1,r1,g1,b1,colorDarkness,false,false);
  
}

void MovingShape::draw(){
  
  updatePos();
  
  if (((MovingObj*)selfObj)->isMaster()) {
    alph = 0.75f;
  }
  if (((MovingObj*)selfObj)->isSlave()) {
    alph = 0.25f;
  }
  // make sure emotions are on the same page
  updateVisuals();

  super::draw();
  
  
}