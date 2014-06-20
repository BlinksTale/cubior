//
//  itemShape.cpp
//  Cubior
//
//  Created by Brian Handy on 5/10/14.
//
//

#include "itemShape.h"
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

void ItemShape::initVisuals(float r1, float g1, float b1, float r2, float g2, float b2, float colorDarkness, bool alt, bool mid) {

    updateColors();
    super::initVisuals(r1,g1,b1,r1,g1,b1,colorDarkness,alt,mid); // make last two false, false?
    
    // fix default shadow status since goal
    shadowState = true;
    directionalCulling = false;
    
    timeSinceCollisionMax = 0.15f;
}

void ItemShape::updateVisuals() {
    
    // Get colors in order
    updateColors();
    
    super::updateVisuals();
    
    // If hit player this turn, reset timeSinceCollision
    if (selfObj->getJustHitPlayer()) {
        timeSinceCollision = timeSinceCollisionMax;
        selfObj->setJustHitPlayer(false); // fixme: really should happen all in selfObj on tick
    }
}

void ItemShape::updateColors() {
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

void ItemShape::draw(){
    
    // make sure emotions are on the same page
    updateVisuals();
    
    // call on cubeShape's function, drawCube, to make a cube visual
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    super::draw();
    // deactivate vertex arrays after drawing
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    
}