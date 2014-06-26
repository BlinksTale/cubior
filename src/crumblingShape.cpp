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
    
    timeSinceCollisionMax = 3.0;
    //timeSinceFirstCollisionMax = 2.0;
}

void CrumblingShape::draw(){//float r1, float g1, float b1, float colorDarkness) {
    
    alph = 1.0-timeSinceCollision/timeSinceCollisionMax;
    
    // make sure emotions are on the same page
    super::updateVisuals();
    
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_BLEND);
	//glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

    
    super::draw();

    /* 
     // Cube's draw function:
     // specify pointer to vertex array
     glColorPointer(3, GL_FLOAT, 0, myColors);
     glVertexPointer(3, GL_FLOAT, 0, myVertices);
     
     glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

     */
    
    //glDisable(GL_BLEND);
    
}
