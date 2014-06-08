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
    colorDefaultR = 0.6;
    colorDefaultG = 0.0;
    colorDefaultB = 0.0;
    colorCurrentR = colorDefaultR;
    colorCurrentG = colorDefaultG;
    colorCurrentB = colorDefaultB;
    updateColors();
    super::initVisuals(r1,g1,b1,r1,g1,b1,colorDarkness,false,false);
    
    // fix default shadow status since goal
    shadowState = true;
    directionalCulling = false;
    
    timeSinceCollisionMax = 0.15f;
}

void CrumblingShape::updateVisuals() {
    super::updateVisuals();
}

void CrumblingShape::draw(){//float r1, float g1, float b1, float colorDarkness) {
    
    // make sure emotions are on the same page
    updateVisuals(); // deleteme?
    
    // If hit player this turn, reset timeSinceCollision
    if (selfObj->getJustHitPlayer()) {
        timeSinceCollision = timeSinceCollisionMax;
        selfObj->setJustHitPlayer(false); // fixme: really should happen all in selfObj on tick
    }
    
    super::draw();
    
}