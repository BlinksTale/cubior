/*
 * flatRender.cpp
 * by Brian Handy
 * 1/20/12
 * 2d Visuals for cubior
 */
#include "flatRender.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Display (name chosen from examples of Dr. Toal & Dr. Dionisio)
void display() {
  // Clear screen w/ black
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw Cubior, the cube!
  glBegin(GL_POLYGON);
    glColor3f(0.6,0.4,0.0); glVertex3f( 0.5,-0.5,0);
    glColor3f(0.6,0.4,0.0); glVertex3f(-0.5,-0.5,0);
    glColor3f(1.0,0.8,0.3); glVertex3f(-0.5, 0.5,0);
    glColor3f(1.0,0.8,0.3); glVertex3f( 0.5, 0.5,0);
  glEnd();

  // End with a quick flush, to draw faster
  glFlush();
}

int renderFlat(int argc, char** argv) { 
// was renderFlat but is now main

  // standard initialization
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  
  // setup & create window
  glutInitWindowPosition(0,0);
  glutInitWindowSize(640,480);
  glutCreateWindow("Cubior");
  
  // Use display for refreshing visuals
  glutDisplayFunc(display);

  // Take input and start processing!
  glutMainLoop();
}