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
#include "goalShape.h"
#include "cubiorShape.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstring>
#include <algorithm>

// Intended Frames Per Second do not change
static const int FPS = 60;
int windowWidth = 640;
int windowHeight = 480;
// Whether to wait for idle to refresh, or force w/ timer
static const bool idleNotTimer = false; // works better, otherwise hangs when PC busy

// Cubior and Cube Count vals (duplicates from Gameplay, will link them later)
const int cubiorNum = cubiorCount;
const int cubeNum = cubeCount;// 9 + playableWidth*playableDepth;

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
static GLfloat cubeCollision[cubeNum];

// Goal object's visual
GoalShape goalShape;
static GLfloat goalX;
static GLfloat goalY;
static GLfloat goalZ;

// Pointers to oft referenced objects
CameraObj* cameraPointer[cubiorNum];

// Display (name chosen from examples of Dr. Toal & Dr. Dionisio)
void display() {
  
  glScissor(0,0,windowWidth,windowHeight);
  glViewport(0,0,windowWidth,windowHeight);

  // Make sure background is black first, then draw on top of it
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // do not understand order, but this works
  
  for (int i=0; i<getCubiorCount(); i++) {
    if (getCubiorPlayable(i)) {
      int w=i%2;
      int h=i>1?0:1;
      // Draw some player i
      glScissor(windowWidth*w/2+(2*w-1),windowHeight*h/2+(2*h-1),windowWidth/2,windowHeight/2);
      glViewport(windowWidth*w/2, windowHeight*h/2, windowWidth*1/2, windowHeight*1/2);
      displayFor(i);
    }
  }

  // End with a quick flush, to draw faster
  glFlush();
  glutSwapBuffers(); // because using double buffer, must swap buffers
}

void displayFor(int player) {
  
  // Paint background cyan to neon blue
  glClearColor(0.3f, 1.0f, 1.0f, 0.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Zoom camera out, then pull back and up to see cubes
  glScalef(0.001,0.001,0.001);
  // old cam position
  //glTranslatef(0,-165,-1550); // better closeup from 0, -100, -1100
  // temp cam position glTranslatef(-playerX[0],-playerY[0]-200,-playerZ[0]-1000);  
  glTranslatef(-1*cameraPointer[player]->getX(),-1*cameraPointer[player]->getY(),-1*cameraPointer[player]->getZ());  


  for (int i=0; i<cubiorNum; i++) { drawPlayer(i); }
  for (int i=0; i<cubeNum; i++) { drawCube(i); }
  drawGoal();

  // Print pause menu
  if (getGameplayRunning()) {
    printString("RUNNING",0,0);
  } else {
/*    char* tempString = "P_ PAUSED";
    tempString[1] = getLastPause();
    cout << tempString << "\n";*/
    printString("PAUSED",0,0);
  }
  // And player stats (wip/temp)
  //if (getPlayer(0)->getGrounded()) { printString("grounded",0,-40); } else { printString("flying",0,-20); }

}

void drawPlayer(int n) {
  if (getCubiorPlayable(n)) {
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
}

void drawCube(int n) {
  glPushMatrix();
  // Move player
  glTranslatef(cubeX[n], cubeY[n], cubeZ[n]);
  int altSize = 400;
  bool alternatingSpot =(
        (cubeX[n]<0)^((int(abs(cubeX[n]+1))%(altSize*2)<altSize))
    ) ^ (
        (cubeY[n]<0)^((int(abs(cubeY[n]+1))%(altSize*2)<altSize))
    )^ (
        (cubeZ[n]<0)^((int(abs(cubeZ[n]+1))%(altSize*2)<altSize))
    );
  float altDark = alternatingSpot * 0.125;
  // And make player bigger
  glScalef(100.0,100.0,100.0);
  cubeShape[n].draw(0.95-0.5*cubeCollision[n]-altDark,1.0-0.5*cubeCollision[n]-altDark,0.5-0.5*cubeCollision[n]-altDark,0.5);
  glPopMatrix();
}

void drawGoal() {
  glPushMatrix();
  // Move player
  glTranslatef(goalX, goalY, goalZ);
  
  // And make player bigger
  glScalef(100.0,100.0,100.0);
  goalShape.drawGoal();
  glPopMatrix();
}
// Leftover Toal Code:
// Handles the window reshape event by first ensuring that the viewport fills
// the entire drawing surface.  Then we use a simple orthographic projection
// with a logical coordinate system ranging from -50..50 in the smaller of
// the width or height, and scale the larger dimension to make the whole
// window isotropic.
void reshape(GLint w, GLint h) {
  windowWidth = w;
  windowHeight = h;
  // working on splitscreen here atm
  // http://nehe.gamedev.net/tutorial/multiple_viewports/20002/
  GLfloat aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); // blank canvas for transforms
  if (windowWidth <= windowHeight) {
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
// updates graphics, and calls itself again
// BASICALLY: main loop for everything
void renderLoop() {
  sendCommands();
  gameplayLoop();
  for (int i=0; i<cubiorNum; i++) {
    updatePlayerGraphic(i);
  }
  for (int i=0; i<cubeNum; i++) {
    updateCubeGraphic(i);
  }
  updateGoalGraphic();
  glutPostRedisplay();
}

void timerRenderLoop(int v) {
  renderLoop();
  glutTimerFunc(1000/FPS, timerRenderLoop, v);
}

void initFlat(int argc, char** argv) { 

  // Initialize Cubior Visual Vals
  for (int i=0; i<cubiorNum; i++) {
    cameraPointer[i] = getCamera(i); // camera for every player
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
    cubeCollision[i] = false;
    cubeShape[i].initVisuals();
    updateCubeGraphic(i);
  }

  // Initialize Goal Visual Vals
  goalX = 0.0;
  goalX = 0.0;
  goalX = 0.0;
  goalShape.initVisuals();
  updateGoalGraphic();

  // standard initialization
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
  
  // setup & create window
  glutInitWindowPosition(0,0);
  glutInitWindowSize(windowWidth,windowHeight);
  glutCreateWindow("Cubior");
  
  // Make sure back faces are behind front faces
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_SCISSOR_TEST); // For splitscreen, must come after Window is created/Init'd

  // input
  glutKeyboardFunc(inputDown);
  glutKeyboardUpFunc(inputUp);
  glutSpecialFunc(specialInputDown);
  glutSpecialUpFunc(specialInputUp);
  glutJoystickFunc(joystickDown, 300);
  glutForceJoystickFunc(); // makes joystick button presses get recognized

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
  setCubeGraphic(n,getCube(n)->getX(),getCube(n)->getY(),getCube(n)->getZ(),getCube(n)->getCollision());
}

void updateGoalGraphic() {
  setGoalGraphic(getGoal()->getX(),getGoal()->getY(),getGoal()->getZ());
}

void setPlayerGraphic(int n, int x, int y, int z) {
  changeX[n] = x - playerX[n];
  changeY[n] = y - playerY[n];
  changeZ[n] = z - playerZ[n];
  playerX[n] = x;
  playerY[n] = y;
  playerZ[n] = z;
}

void setCubeGraphic(int n, int x, int y, int z, bool b) {
  cubeX[n] = x;
  cubeY[n] = y;
  cubeZ[n] = z;
  cubeCollision[n] = b;
}

void setGoalGraphic(int x, int y, int z) {
  goalX = x;
  goalY = y;
  goalZ = z;
}

void updateFlat() {
  
}

void renderFlat() {
  // will refresh screen, right?
}

void printString(const char *string, int x, int y) {
  int len, i;
  len = (int)strlen(string);
  glColor3f( 0.0f, 0.0f, 0.0f );
  glRasterPos3f(-4.5f*len+x,200.0f+y,590.0f);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
    }
}
