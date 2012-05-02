/*
 * cubeShape.cpp
 * by Brian Handy
 * 2/2/12
 * 3d cube model
 */
#include "cubeShape.h"
#include "flatRender.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h> // for pauseText
#include <time.h> // for printing timestamps
#include <sys/time.h> // for linux time


void CubeShape::initVisuals(float nR, float nG, float nB, float nR2, float nG2, float nB2, float colorDarkness, bool alt, bool mid) {
  midFloor = mid;
  alternatingSpot = alt;
  altDark = alternatingSpot * 0.125;
  // Nothing here yet! Colors are what distinguish Cubiors from Cubes atm
  r1 = nR - altDark;
  g1 = nG - altDark;
  b1 = nB - altDark;
  r2 = nR - colorDarkness - altDark;
  g2 = nG - colorDarkness - altDark;
  b2 = nB - colorDarkness - altDark;
  r3 = nR2 - altDark;
  g3 = nG2 - altDark;
  b3 = nB2 - altDark;
}

void CubeShape::updateVisuals() {
  // This handles colors and face smile for Cubiors, but nothing for Cubes yet
}

void CubeShape::draw() {

struct timeval tim;

    int c1,c2,c3,c4,c5,c6,c7,c8,c9;
    if (getTiming()) { gettimeofday(&tim, NULL); c1 = (tim.tv_sec+(tim.tv_usec/1.0)); }

  // make sure visuals are on the same page
  updateVisuals();

    if (getTiming()) { gettimeofday(&tim, NULL); c2 = (tim.tv_sec+(tim.tv_usec/1.0)); }



    if (getTiming()) { gettimeofday(&tim, NULL); c3 = (tim.tv_sec+(tim.tv_usec/1.0)); }
    if (!useNeighbors || !neighbors[5]) { // 5 must be back since 4 is front
// Draw Cubior, the cube!
    // Back
    glBegin(GL_TRIANGLES);
    glColor3f(r2-0.2,g2-0.2,b2-0.2);
    glVertex3f( 0.5,-0.5,-0.5);
    glColor3f(r1-0.2,g1-0.2,b1-0.2);
    glVertex3f( 0.5, 0.5,-0.5);
    glVertex3f(-0.5, 0.5,-0.5);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.5, 0.5,-0.5);
    glColor3f(r2-0.2,g2-0.2,b2-0.2);
    glVertex3f(-0.5,-0.5,-0.5);
    glVertex3f( 0.5,-0.5,-0.5);
    glEnd();

    if (getTiming()) { gettimeofday(&tim, NULL); c4 = (tim.tv_sec+(tim.tv_usec/1.0)); }

}
    if (!useNeighbors || !neighbors[3]) { // 3 is bottom since 2 is top
    // Bottom
    glColor3f(r2,g2,b2);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.5,-0.5, 0.5);
    glVertex3f( 0.5,-0.5, 0.5);
    glVertex3f(-0.5,-0.5,-0.5);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f( 0.5,-0.5, 0.5);
    glVertex3f( 0.5,-0.5,-0.5);
    glVertex3f(-0.5,-0.5,-0.5);
    glEnd();

    if (getTiming()) { gettimeofday(&tim, NULL); c5 = (tim.tv_sec+(tim.tv_usec/1.0)); }

}
    if (!useNeighbors || !neighbors[4]) { // 4 is front side
    // Front
    glBegin(GL_TRIANGLES);
    glColor3f(r2,g2,b2); // can't guarantee it will exist above, since bottom may not be drawn
    glVertex3f( 0.5,-0.5,0.5);
    glVertex3f(-0.5,-0.5,0.5);
    glColor3f(r1,g1,b1);
    glVertex3f( 0.5, 0.5,0.5);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.5, 0.5,0.5);
    glVertex3f( 0.5, 0.5,0.5);
    glColor3f(r2,g2,b2);
    glVertex3f(-0.5,-0.5,0.5);
    glEnd();

    if (getTiming()) { gettimeofday(&tim, NULL); c6 = (tim.tv_sec+(tim.tv_usec/1.0)); }

}
    if (!useNeighbors || !neighbors[0]) { // 0 is left
    // Left
    glBegin(GL_TRIANGLES);
    glColor3f(r2,g2,b2);
    glVertex3f( 0.5,-0.5,-0.5);
    glVertex3f( 0.5,-0.5, 0.5);
    glColor3f(r1,g1,b1);
    glVertex3f( 0.5, 0.5,-0.5);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f( 0.5, 0.5, 0.5);
    glVertex3f( 0.5, 0.5,-0.5);
    glColor3f(r2,g2,b2);
    glVertex3f( 0.5,-0.5, 0.5);
    glEnd();

    if (getTiming()) { gettimeofday(&tim, NULL); c7 = (tim.tv_sec+(tim.tv_usec/1.0)); }

}
    if (!useNeighbors || !neighbors[1]) { // 1 is right
    // Right
    glBegin(GL_TRIANGLES);
    glColor3f(r2,g2,b2);
    glVertex3f(-0.5,-0.5,-0.5);
    glColor3f(r1,g1,b1);
    glVertex3f(-0.5, 0.5,-0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.5, 0.5, 0.5);
    glColor3f(r2,g2,b2);
    glVertex3f(-0.5,-0.5, 0.5);
    glVertex3f(-0.5,-0.5,-0.5);
    glEnd();

    if (getTiming()) { gettimeofday(&tim, NULL); c8 = (tim.tv_sec+(tim.tv_usec/1.0)); }
}
    if (!useNeighbors || !neighbors[2]) { // 2 is above
    // Top
    glColor3f(r3,g3,b3);
    glBegin(GL_TRIANGLES);
    glVertex3f( 0.5, 0.5,-0.5);
    glVertex3f( 0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f( 0.5, 0.5,-0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5,-0.5);
    glEnd();
}
    if (getTiming()) { gettimeofday(&tim, NULL); c9 = (tim.tv_sec+(tim.tv_usec/1.0)); }

if (getTiming() && 100<(c9-c1)) {//(c9 - c1 > 100) {
  printf("INSIDE...\n");
  printf("update Visuals: %d\n",c2-c1);
  printf("set colors: %d\n",c3-c2);
  printf("back: %d\n",c4-c3);
  printf("bottom: %d\n",c5-c4);
  printf("front: %d\n",c6-c5);
  printf("left: %d\n",c7-c6);
  printf("right: %d\n",c8-c7);
  printf("top: %d\n",c9-c8);
  printf("total: %d\n",c9-c1);
}

}

void CubeShape::setNeighbors(bool newNeighbors[6]) { 
  for (int i=0; i< 6; i++) {
  neighbors[i] = newNeighbors[i];
  }
}
