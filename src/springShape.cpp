//
//  springShape.cpp
//  Cubior
//
//  Created by Brian Handy on 4/12/14.
//
//

#include "cubiorShape.h"
#include "cubeObj.h"
#include "gameplay.h"
#include "springShape.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <iostream> // for cout
#define _USE_MATH_DEFINES
#include <math.h>

float springOffsetMax = .8f;

void SpringShape::initVisuals() {
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
    
    timeSinceCollisionMax = 0.15f;
}

void SpringShape::draw(){
    
    // make sure emotions are on the same page
    updateVisuals();

    float springOffset = sin(timeSinceCollision/timeSinceCollisionMax*M_PI)*springOffsetMax;
    
    for (int i=0; i<24; i++) {
        if (i % 12 == 1 || i % 12 == 4) {
            myVertices[i] += springOffset; // extend one to the top
        }
    }
    
    super::draw();
    
    for (int i=0; i<24; i++) {
        if (i % 12 == 1 || i % 12 == 4) {
            myVertices[i] -= springOffset; // extend one to the top
        }
    }

}
