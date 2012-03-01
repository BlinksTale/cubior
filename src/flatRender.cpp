/*
 * flatRender.cpp
 * by Brian Handy
 * 1/20/12
 * 2d Visuals for cubior
 */
#include "flatRender.h"
#include "gameplay.h"
#include "keyboard.h"
#include "cubeShape.h"
#include "cubiorShape.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <algorithm>

// Cubior and Cube Count vals (duplicates from Gameplay, will link them later)
const int cubiorNum = 3;
const int cubeNum = 9;

// Intended Frames Per Second do not change
static const int FPS = 60;
// Whether to wait for idle to refresh, or force w/ timer
static const bool idleNotTimer = false; // works better, otherwise hangs when PC busy

// angle of cubior while he rotates
static float playerAngleNumerator[cubiorNum];
static float playerAngleDivisor[cubiorNum];

// pos of cubiors while they move
CubiorShape cubiorShape[cubiorNum];
static GLfloat playerX[cubiorNum];
static GLfloat playerY[cubiorNum];
static GLfloat playerZ[cubiorNum];
static GLfloat changeX[cubiorNum];
static GLfloat changeY[cubiorNum];
static GLfloat changeZ[cubiorNum];
static GLfloat lastChangeZ[cubiorNum];

// pos of cube obstacles
//static int cubesTotal = 1;
CubeShape cubeShape[cubeNum];
static GLfloat cubeX[cubeNum];
static GLfloat cubeY[cubeNum];
static GLfloat cubeZ[cubeNum];

// Display (name chosen from examples of Dr. Toal & Dr. Dionisio)
void display() {
  
  // Clear screen w/ black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); // HELP: need a refresher on how glLoadIdentity is used

  // Zoom camera out
  glScalef(0.001,0.001,0.001);
  // then pull back and up to see cubes
  glTranslatef(0,-100,-1100);
  
  for (int i=0; i<cubiorNum; i++) {
    drawPlayer(i);
  }
  for (int i=0; i<cubeNum; i++) {
    drawCube(i);
  }

  // End with a quick flush, to draw faster
  glFlush();
  glutSwapBuffers(); // because using double buffer, must swap buffers
}

void drawPlayer(int n) {
  glPushMatrix();
  // Move player
  glTranslatef(playerX[n], playerY[n], playerZ[n]);
  // Then animate player rotation
  if (changeX[n] > 2.0 || changeX[n] < -2.0 || changeZ[n] > 2.0 || changeZ[n] < -2.0) {
    playerAngleDivisor[n] = ((changeZ[n] != 0.0) ? changeZ[n] : 1.0);
    playerAngleNumerator[n] = changeX[n];
    lastChangeZ[n] = changeZ[n];
  }
  if (lastChangeZ[n] < 0.0) {
    glRotatef(atan(playerAngleNumerator[n]/playerAngleDivisor[n])*60.0 + 180,0.0,1.0,0.0);
  } else {
    glRotatef(atan(playerAngleNumerator[n]/playerAngleDivisor[n])*60.0,0.0,1.0,0.0);
  }
  // And make player bigger
  glScalef(100.0,100.0,100.0);
  cubiorShape[n].drawCubior(n);
  glPopMatrix();
}

void drawCube(int n) {
  glPushMatrix();
  // Move player
  glTranslatef(cubeX[n], cubeY[n], cubeZ[n]);
  
  // And make player bigger
  glScalef(100.0,100.0,100.0);
  cubeShape[n].drawCube(0.95,1.0,0.5,0.5);
  glPopMatrix();
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
    gluPerspective(45.0, aspect, 0.50, 10.0);
  } else {
    // height is smaller, go from -50 .. 50 in height
    //glFrustum(-50.0*aspect, 50.0*aspect, -50.0, 50.0, -1.0, 1.0);
    gluPerspective(45.0, aspect, 0.50, 10.0);
  }
}

// main loop for rendering. Also calls gameplay loop,
// updates graphisc, and calls itself again
void renderLoop() {
  sendCommands();
  gameplayLoop();
  for (int i=0; i<cubiorNum; i++) {
    updatePlayerGraphic(i);
  }
  for (int i=0; i<cubeNum; i++) {
    updateCubeGraphic(i);
  }
  glutPostRedisplay();
}

void timerRenderLoop(int v) {
  renderLoop();
  glutTimerFunc(1000/FPS, timerRenderLoop, v);
}

void initFlat(int argc, char** argv) { 

  // Initialize Cubior Visual Vals
  for (int i=0; i<cubiorNum; i++) {
    playerAngleNumerator[i] = 1.0;
    playerAngleDivisor[i] = 1.0;
    playerX[i] = 0.0;
    playerY[i] = 0.0;
    playerZ[i] = 0.0;
    changeX[i] = 0.0;
    changeY[i] = 0.0;
    changeZ[i] = 0.0;
    lastChangeZ[i] = 0.0;
    cubiorShape[i].initCubiorVisuals(i);
    updatePlayerGraphic(i);
  }

  // Initialize Cube Visual Vals
  for (int i=0; i<cubeNum; i++) {
    cubeX[i] = 0.0;
    cubeY[i] = 0.0;
    cubeZ[i] = 0.0;
    cubeShape[i].initCubeVisuals();
    updateCubeGraphic(i);
  }

  // standard initialization
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
  
  // setup & create window
  glutInitWindowPosition(0,0);
  glutInitWindowSize(640,480);
  glutCreateWindow("Cubior");
  
  // Make sure back faces are behind front faces
  glEnable(GL_DEPTH_TEST);
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

void updatePlayerGraphic(int n) {
  setPlayerGraphic(n,getPlayer(n)->getX(),getPlayer(n)->getY(),getPlayer(n)->getZ());
  //setPlayerGraphic(n,getCube(0)->getX(),getCube(0)->getY(),getCube(0)->getZ());
  //setPlayerGraphic(2000,0,-1000,0);
}

void updateCubeGraphic(int n) {
  setCubeGraphic(n,getCube(n)->getX(),getCube(n)->getY(),getCube(n)->getZ());
}

void setPlayerGraphic(int n, int x, int y, int z) {
  changeX[n] = x - playerX[n];
  changeY[n] = y - playerY[n];
  changeZ[n] = z - playerZ[n];
  playerX[n] = x;
  playerY[n] = y;
  playerZ[n] = z;
}

void setCubeGraphic(int n, int x, int y, int z) {
  cubeX[n] = x;
  cubeY[n] = y;
  cubeZ[n] = z;
}

void updateFlat() {
  
}

void renderFlat() {
  // will refresh screen, right?
}
