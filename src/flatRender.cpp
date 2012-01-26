/*
 * flatRender.cpp
 * by Brian Handy
 * 1/20/12
 * 2d Visuals for cubior
 */
#include "flatRender.h"
#include "gameplay.h"
#include "keyboard.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Intended Frames Per Second do not change
static const int FPS = 60;
// Whether to wait for idle to refresh, or force w/ timer
static const bool idleNotTimer = false; // works better, otherwise hangs when PC busy

// angle of cubior while he rotates
static GLfloat playerAngle = 45.0;

// pos of cubior while he moves
static GLfloat playerX = 0.0;
static GLfloat playerY = 0.0;

// Display (name chosen from examples of Dr. Toal & Dr. Dionisio)
void display() {
  // Clear screen w/ black
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); // HELP: need a refresher on how glLoadIdentity is used

  // Zoom camera out
  glScalef(0.1,0.1,0.1);
  // Then move player
  glTranslatef(playerX, playerY, 0.0);
  // Then animate player
  glRotatef(playerAngle, 0.0, 0.0, 1.0);
  // And make player bigger
  glScalef(100.0,100.0,100.0);

  glPushMatrix();
  // Draw Cubior, the cube!
  glBegin(GL_POLYGON);
    glColor3f(0.6,0.4,0.0); glVertex3f( 0.5,-0.5,0);
    glColor3f(0.6,0.4,0.0); glVertex3f(-0.5,-0.5,0);
    glColor3f(1.0,0.8,0.3); glVertex3f(-0.5, 0.5,0);
    glColor3f(1.0,0.8,0.3); glVertex3f( 0.5, 0.5,0);
  glEnd();
  // Mouth
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f( 0.3,-0.25,0);
    glColor3f(0,0,0); glVertex3f(-0.3,-0.25,0);
    glColor3f(0,0,0); glVertex3f(-0.3,-0.15,0);
    glColor3f(0,0,0); glVertex3f( 0.3,-0.15,0);
  glEnd();
  // Eye L
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f( 0.25, 0.3,0);
    glColor3f(0,0,0); glVertex3f( 0.15, 0.3,0);
    glColor3f(0,0,0); glVertex3f( 0.15, 0.0,0);
    glColor3f(0,0,0); glVertex3f( 0.25, 0.0,0);
  glEnd();
  // Eye R
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(-0.25, 0.3,0);
    glColor3f(0,0,0); glVertex3f(-0.15, 0.3,0);
    glColor3f(0,0,0); glVertex3f(-0.15, 0.0,0);
    glColor3f(0,0,0); glVertex3f(-0.25, 0.0,0);
  glEnd();
  glPopMatrix();

  // End with a quick flush, to draw faster
  glFlush();
  glutSwapBuffers(); // because using double buffer, must swap buffers
}

// Leftover Toal Code:
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

// main loop for rendering. Also calls gameplay loop,
// updates graphisc, and calls itself again
void renderLoop() {
  sendCommands();
  gameplayLoop();
  updatePlayerGraphic();
  glutPostRedisplay();
}
void timerRenderLoop(int v) {
  renderLoop();
  glutTimerFunc(1000/FPS, timerRenderLoop, v);
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

  // input
  glutKeyboardFunc(inputDown);
  glutKeyboardUpFunc(inputUp);
  glutSpecialFunc(specialInputDown);
  glutSpecialUpFunc(specialInputUp);
  
  // Use display for refreshing visuals
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  if (idleNotTimer) {
    glutIdleFunc(renderLoop);
  } else {
    glutTimerFunc(100, timerRenderLoop, 0);
  }
  // Take input and start processing!
  glutMainLoop();
}

void updatePlayerGraphic() {
  setPlayerGraphic(getPlayerX(),getPlayerY(),getPlayerAngleZ());
}

void setPlayerGraphic(int x, int y, int angle) {
  playerX = x;
  playerY = y;
  playerAngle = angle;
}

void updateFlat() {
  
}

void renderFlat() {
  // will refresh screen, right?
}
