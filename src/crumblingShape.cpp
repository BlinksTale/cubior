//
//  crumblingShape.cpp
//  Cubior
//
//  Created by Brian Handy on 5/10/14.
//
//

#include "crumblingShape.h"
#include "cubiorShape.h"
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

void CrumblingShape::initVisuals() {
    colorDarkness = 0.4;
    colorDefaultA = 0.4;
    colorDefaultR = 0.5;
    colorDefaultG = 0.5;
    colorDefaultB = 0.5;
    colorCurrentR = colorDefaultR;
    colorCurrentG = colorDefaultG;
    colorCurrentB = colorDefaultB;
    super::updateColors();
    super::initVisuals(r1,g1,b1,r1,g1,b1,colorDarkness,false,false);
    
}

void CrumblingShape::draw(){//float r1, float g1, float b1, float colorDarkness) {
    
    // make sure emotions are on the same page
    super::updateVisuals();
    
    super::draw();
    
}
