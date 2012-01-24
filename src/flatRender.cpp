/*
 * flatRender.cpp
 * by Brian Handy
 * 1/20/12
 * 2d Visuals for cubior
 */
#include "flatRender.h"
#include "gameplay.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Intended Frames Per Second do not change
static const int FPS = 60;

// angle of cubior while he rotates
static GLfloat angle = 90.0;

// pos of cubior while he moves
static GLfloat playerX = 0.0;
static GLfloat playerY = 0.0;

// Display (name chosen from examples of Dr. Toal & Dr. Dionisio)
void display() {
  // Clear screen w/ black
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); // HELP: need a refresher on how glLoadIdentity is used

  glTranslatef(playerX, playerY, 0.0);
  glRotatef(angle*2, 0.0, 0.0, 1.0);

  glPushMatrix();
  // Draw Cubior, the cube!
  glBegin(GL_POLYGON);
    glColor3f(0.6,0.4,0.0); glVertex3f( 05,-05,0);
    glColor3f(0.6,0.4,0.0); glVertex3f(-05,-05,0);
    glColor3f(1.0,0.8,0.3); glVertex3f(-05, 05,0);
    glColor3f(1.0,0.8,0.3); glVertex3f( 05, 05,0);
  glEnd();
  glPopMatrix();

  // End with a quick flush, to draw faster
  glFlush();
  glutSwapBuffers(); // because using double buffer, must swap buffers
}

// Handles the window reshape event by first ensuring that the viewport fills
// the entire drawing surface.  Then we use a simple orthographic projection
// with a logical coordinate system ranging from -50..50 in the smaller of
// the width or height, and scale the larger dimension to make the whole
// window isotropic.
void reshape(GLint w, GLint h) {
  glViewport(0, 0, w, h);
  GLfloat aspect = (GLfloat)w / (GLfloat)h;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h) {
    // width is smaller, go from -50 .. 50 in width
    glOrtho(-50.0, 50.0, -50.0/aspect, 50.0/aspect, -1.0, 1.0);
  } else {
    // height is smaller, go from -50 .. 50 in height
    glOrtho(-50.0*aspect, 50.0*aspect, -50.0, 50.0, -1.0, 1.0);
  }
}

// Handles the timer by incrementing the angle of rotation and requesting the
// window to display again, provided the program is in the spinning state.
// Since the timer function is only called once, it sets the same function to
// be called again.
void timer(int v) {
  gameplayLoop();
  updatePlayerGraphic();
  glutPostRedisplay();
  glutTimerFunc(1000/FPS, timer, v);
}

void initFlat(int argc, char** argv) { 
// was renderFlat but is now main
  // start with player position
  updatePlayerGraphic();

  // standard initialization
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  
  // setup & create window
  glutInitWindowPosition(0,0);
  glutInitWindowSize(640,480);
  glutCreateWindow("Cubior");
  
  // Use display for refreshing visuals
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutTimerFunc(100, timer, 0);
  // Take input and start processing!
  glutMainLoop();
}

void updatePlayerGraphic() {
  setPlayerGraphic(getPlayerX(),getPlayerY());
}

void setPlayerGraphic(int x, int y) {
  playerX = x;
  playerY = y;
}

void updateFlat() {
  
}

void renderFlat() {
  // will refresh screen, right?
}
