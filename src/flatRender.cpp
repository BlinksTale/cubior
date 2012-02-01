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

#include <cmath>
#include <algorithm>

// Intended Frames Per Second do not change
static const int FPS = 60;
// Whether to wait for idle to refresh, or force w/ timer
static const bool idleNotTimer = false; // works better, otherwise hangs when PC busy

// Face smile height
static float faceSmileValue = 0.1;
static float faceSmileHeight = -0.2;
static float faceLineWidth = 0.1;
static float faceSmileWidth = 0.8;
static float faceSmileCurveWidth = 0.2;
static float faceSmileCurveHeight = 0.2;

static float colorDarkness = 0.4;
static float colorDefaultA = 0.4;
static float colorDefaultR = 0.2;
static float colorDefaultG = 0.0;
static float colorDefaultB =-0.4;
float colorCurrentR = colorDefaultR;
float colorCurrentG = colorDefaultG;
float colorCurrentB = colorDefaultB;

// angle of cubior while he rotates
static GLfloat playerAngle = 45.0;

// pos of cubior while he moves
static GLfloat playerX = 0.0;
static GLfloat playerY = 0.0;
static GLfloat playerZ = 0.0;
static GLfloat changeX = 0.0;
static GLfloat changeY = 0.0;
static GLfloat changeZ = 0.0;

// Display (name chosen from examples of Dr. Toal & Dr. Dionisio)
void display() {
  
  if (getInvincibility()) {
    colorCurrentR += (rand() % 4 - 2) / 4.0;
    colorCurrentG += (rand() % 4 - 2) / 4.0;
    colorCurrentB += (rand() % 4 - 2) / 4.0;
    if (colorCurrentR > 1.0) { colorCurrentR = 1.0; }
    if (colorCurrentG > 1.0) { colorCurrentG = 1.0; }
    if (colorCurrentB > 1.0) { colorCurrentB = 1.0; }
    if (colorCurrentR < colorDefaultA) { colorCurrentR = colorDefaultA; }
    if (colorCurrentG < colorDefaultA) { colorCurrentG = colorDefaultA; }
    if (colorCurrentB < colorDefaultA) { colorCurrentB = colorDefaultA; }
  } else { colorCurrentR = colorDefaultA; colorCurrentG = colorDefaultA; colorCurrentB = colorDefaultA; }

  if (!getLocking()) { colorCurrentR += colorDefaultR; colorCurrentG += colorDefaultG; colorCurrentB += colorDefaultB; }
  
  float r1 = colorCurrentR + colorDarkness;
  float g1 = colorCurrentG + colorDarkness;
  float b1 = colorCurrentB + colorDarkness;
  float r2 = colorCurrentR;
  float g2 = colorCurrentG;
  float b2 = colorCurrentB;
  
  // Clear screen w/ black
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); // HELP: need a refresher on how glLoadIdentity is used

  // Zoom camera out
  glScalef(0.001,0.001,0.001);
  // Then move player
  glTranslatef(playerX, playerY, playerZ);
  // Something broken here, will fix it later
  // Then animate player
  float atanDivisor = ((changeZ != 0.0) ? changeZ : 1.0);
  float atanNumerator = changeX;
  if (changeZ < 0.0) {
    glRotatef(atan(atanNumerator/atanDivisor)*60.0 + 180,0.0,1.0,0.0);
  } else {
    glRotatef(atan(atanNumerator/atanDivisor)*60.0,0.0,1.0,0.0);
  }
  // And make player bigger
  glScalef(100.0,100.0,100.0);

  glPushMatrix();
  // Draw Cubior, the cube!
    // Back
    glBegin(GL_POLYGON);
    glColor3f(r1-0.2,g1-0.2,b1-0.2); glVertex3f( 0.5, 0.5,-0.5);
    glColor3f(r1-0.2,g1-0.2,b1-0.2); glVertex3f(-0.5, 0.5,-0.5);
    glColor3f(r2-0.2,g2-0.2,b2-0.2); glVertex3f(-0.5,-0.5,-0.5);
    glColor3f(r2-0.2,g2-0.2,b2-0.2); glVertex3f( 0.5,-0.5,-0.5);
    glEnd();
    // Left
    glBegin(GL_POLYGON);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5,-0.5);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5, 0.5);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5, 0.5);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5,-0.5);
    glEnd();
    // Right
    glBegin(GL_POLYGON);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5,-0.5);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5, 0.5);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5, 0.5);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5,-0.5);
    glEnd();
    // Bottom
    glBegin(GL_POLYGON);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5, 0.5);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5, 0.5);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5,-0.5);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5,-0.5);
    glEnd();
    // Top
    glBegin(GL_POLYGON);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5,-0.5);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5, 0.5);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5, 0.5);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5,-0.5);
    glEnd();
    // Front
    glBegin(GL_POLYGON);
    glColor3f(r2,g2,b2); glVertex3f( 0.5,-0.5,0.5);
    glColor3f(r2,g2,b2); glVertex3f(-0.5,-0.5,0.5);
    glColor3f(r1,g1,b1); glVertex3f(-0.5, 0.5,0.5);
    glColor3f(r1,g1,b1); glVertex3f( 0.5, 0.5,0.5);
    glEnd();

  // Mouth
  if (!getLocking()) {
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2,                       faceSmileHeight + faceLineWidth/2 + faceSmileValue,0.5); // top right
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2,                       faceSmileHeight - faceLineWidth/2 + faceSmileValue,0.5); // bot right
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2,0.5); // bot left
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2,0.5); // top left
  glEnd();
  }
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2,0.5); // top right
    glColor3f(0,0,0); glVertex3f( faceSmileWidth/2 - faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2,0.5); // bot right
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2,0.5); // bot left
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2,0.5); // top left
  glEnd();
  if (!getLocking()) {
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2,                       faceSmileHeight + faceLineWidth/2 + faceSmileValue,0.5); // top left
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight + faceLineWidth/2,0.5); // top right
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2 + faceSmileCurveWidth, faceSmileHeight - faceLineWidth/2,0.5); // bot right
    glColor3f(0,0,0); glVertex3f(-faceSmileWidth/2,                       faceSmileHeight - faceLineWidth/2 + faceSmileValue,0.5); // bot left
  glEnd();
  }
  // Eye L
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f( 0.25, 0.3,0.5);
    glColor3f(0,0,0); glVertex3f( 0.25, 0.0,0.5);
    glColor3f(0,0,0); glVertex3f( 0.15, 0.0,0.5);
    glColor3f(0,0,0); glVertex3f( 0.15, 0.3,0.5);
  glEnd();
  // Eye R
  glBegin(GL_POLYGON);
    glColor3f(0,0,0); glVertex3f(-0.25, 0.3,0.5);
    glColor3f(0,0,0); glVertex3f(-0.15, 0.3,0.5);
    glColor3f(0,0,0); glVertex3f(-0.15, 0.0,0.5);
    glColor3f(0,0,0); glVertex3f(-0.25, 0.0,0.5);
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
    //glFrustum(-50.0, 50.0, -50.0/aspect, 50.0/aspect, -1.0, 1.0);
    gluPerspective(45.0, aspect, 0.0, 10.0);
  } else {
    // height is smaller, go from -50 .. 50 in height
    //glFrustum(-50.0*aspect, 50.0*aspect, -50.0, 50.0, -1.0, 1.0);
    gluPerspective(45.0, aspect, 0.0, 10.0);
  }
}

// main loop for rendering. Also calls gameplay loop,
// updates graphisc, and calls itself again
void renderLoop() {
  sendCommands();
  gameplayLoop();
  updatePlayerGraphic();
  glutPostRedisplay();
  faceSmileValue = (getHappiness()-0.5)/10;
}
void timerRenderLoop(int v) {
  renderLoop();
  glutTimerFunc(1000/FPS, timerRenderLoop, v);
}

void initFlat(int argc, char** argv) { 
  srand(1);

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
  /* Not working, don't know why. Kind of important long term
  */
  // Make sure back faces are behind front faces
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

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
  setPlayerGraphic(getPlayerX(),getPlayerY(),getPlayerZ(),getPlayerAngleZ());
}

void setPlayerGraphic(int x, int y, int z, int angle) {
  changeX = x - playerX;
  changeY = y - playerY;
  changeZ = z - playerZ;
  playerX = x;
  playerY = y;
  playerZ = z;
  playerAngle = angle;
}

void updateFlat() {
  
}

void renderFlat() {
  // will refresh screen, right?
}
