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

#include <cmath> // for rand
#include <cstring>
#include <algorithm>
#include <stdio.h> // for pauseText
#include <time.h> // for printing timestamps
#include <sys/time.h> // for linux time

// Intended Frames Per Second do not change
static const int FPS = 60;
int windowWidth = 640;
int windowHeight = 480;
// Whether to wait for idle to refresh, or force w/ timer
static const bool refreshOnIdle = false; // works better, otherwise hangs when PC busy

// Cubior and Cube Count vals (duplicates from Gameplay, will link them later)
const int cubiorNum = cubiorCount;
const int cubeNum = cubeCount;// 9 + playableWidth*playableDepth;
char pausedText[50];

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

// time stuff
bool timing = false;
struct timeval tim;
int lastTime1 = 0;
int lastTime2 = 0;
int lastTime3 = 0;
int lastTime4 = 0;
int lastTime5 = 0;

// Goal object's visual
GoalShape goalShape;
static GLfloat goalX;
static GLfloat goalY;
static GLfloat goalZ;

// Pointers to oft referenced objects
CameraObj* cameraPointer[cubiorNum];

// Display (name chosen from examples of Dr. Toal & Dr. Dionisio)
void display() {

  int dTime1, dTime2, dTtime3, dTime4, dTime5, dTime6;
  
  if (timing) {
  printf("------------\n");
  gettimeofday(&tim, NULL);
  int dTime1 = (tim.tv_sec+(tim.tv_usec/1.0));
  lastTime2 = dTime1;
  }
  
  glScissor(0,0,windowWidth,windowHeight);
  glViewport(0,0,windowWidth,windowHeight);

  // Make sure background is black first, then draw on top of it
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // do not understand order, but this works
  
  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime2 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("preppingScreen time: %d\n",dTime2-dTime1);
  }
  
  for (int i=0; i<getCubiorCount(); i++) {
    if (getCubiorPlayable(i)) {
      int cT1, cT2, cT3, cT4, cT5;
      if (timing) {
      printf("=====CUBIOR DRAW BEGIN=======\n");
      gettimeofday(&tim, NULL);
      int cT1 = (tim.tv_sec+(tim.tv_usec/1.0));
      }
      
      int w=i%2;
      int h=i>1?1:0;
      int altX = (h)*2+1-w; // side screen neighbor
      int altY = (1-h)*2+w; // elevator screen neighbor
      int altXY = getCubiorCount()-i-1; // diagonal screen neighbor
      int posX = windowWidth*w/2 +(2*w-1); // screen position
      int posY = windowHeight*(1-h)/2+(2*(1-h)-1); // screen position
      int viewW = windowWidth*1/2;
      int viewH = windowHeight*1/2;

  if (timing) {
      gettimeofday(&tim, NULL);
      int cT2 = (tim.tv_sec+(tim.tv_usec/1.0));
      printf("prep Cubior time: %d\n",cT2-cT1);
  }
  
      // Now fill in empty space
      if (!getCubiorPlayable(altX)
      &&!getCubiorPlayable(altY)
      &&!getCubiorPlayable(altXY)) {
        // All alone? Fill it all!
        viewW *= 2;
        viewH *= 2;
        posX = 0;
        posY = 0;
        setPerspective(2,2);
      } else if (!getCubiorPlayable(altY) && !getCubiorPlayable(altXY)) {
        // Space Vertically? Fill the height
        viewH *= 2;
        posY = 0;
        setPerspective(1,2);
      } else if (!getCubiorPlayable(altX)) {
        // Space Horizontally? Fill the width
        viewW *= 2;
        posX = 0;
        setPerspective(2,1);
      } else {
        // Nothing abnormal! As you were
        setPerspective(1,1);
      }

  if (timing) {
      gettimeofday(&tim, NULL);
      int cT3 = (tim.tv_sec+(tim.tv_usec/1.0));
      printf("splitscreen fill time: %d\n",cT3-cT2);
  }
  
      // Draw some player i
      glScissor(posX,posY,viewW,viewH);

  if (timing) {
      gettimeofday(&tim, NULL);
      int cT4 = (tim.tv_sec+(tim.tv_usec/1.0));
      printf("scissor time: %d\n",cT4-cT3);
  }
  
      glViewport(posX, posY, viewW, viewH);

  if (timing) {
      gettimeofday(&tim, NULL);
      int cT5 = (tim.tv_sec+(tim.tv_usec/1.0));
      printf("viewport time: %d\n",cT5-cT4);
  }
      displayFor(i);

  if (timing) {
      gettimeofday(&tim, NULL);
      int cT6 = (tim.tv_sec+(tim.tv_usec/1.0));
      printf("displayFor time: %d\n",cT6-cT5);
  }
  
    }
    
  if (timing) {
  printf("======CUBIOR DRAW END========\n");
  }
  }

  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime3 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("drawCubiors time: %d\n",dTime3-dTime2);
  }
  
  // End with a quick flush, to draw faster
  glFlush();
  glutSwapBuffers(); // because using double buffer, must swap buffers

  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime4 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("totalDrawTime() time: %d\n",dTime4-dTime1);
  printf("------------\n");
  }

}

void displayFor(int player) {
  
  int c1,c2,c3,c4,c5;
  if (timing) {
  printf("&&&&& DISPLAYFOR CUBIOR BEGIN &&&&&&&\n");

  gettimeofday(&tim, NULL);
  int c1 = (tim.tv_sec+(tim.tv_usec/1.0));
  }
  
  // Paint background cyan to neon blue
  glClearColor(0.3f, 1.0f, 1.0f, 0.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (timing) {
  gettimeofday(&tim, NULL);
  int c2 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("prep time: %d\n",c2-c1);
  }
  
  // Zoom camera out, then pull back and up to see cubes
  glScalef(0.001,0.001,0.001);
  // old cam position
  //glTranslatef(0,-165,-1550); // better closeup from 0, -100, -1100
  // temp cam position glTranslatef(-playerX[0],-playerY[0]-200,-playerZ[0]-1000);
  glRotatef(-1*cameraPointer[player]->getAngleX(),1.0,0.0,0.0);
  glRotatef(-1*cameraPointer[player]->getAngleY(),0.0,1.0,0.0);
  glRotatef(-1*cameraPointer[player]->getAngleZ(),0.0,0.0,1.0);
  glTranslatef(-1*cameraPointer[player]->getX(),-1*cameraPointer[player]->getY(),-1*cameraPointer[player]->getZ());

/*  gluLookAt(
    1*cameraPointer[player]->getX(),
    1*cameraPointer[player]->getY(),
    1*cameraPointer[player]->getZ(),
    playerX[player],
    playerY[player],
    playerZ[player],
    0,1,0
  );
*/
  if (timing) {
  gettimeofday(&tim, NULL);
  int c3 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("moveCam time: %d\n",c3-c2);
  }
  
  for (int i=0; i<cubiorNum; i++) { drawPlayer(i); }

  if (timing) {
  gettimeofday(&tim, NULL);
  int c4 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("drawPlayer time: %d\n",c4-c3);
  }
  
  for (int i=0; i<cubeNum; i++) { drawCube(i); }

  if (timing) {
  gettimeofday(&tim, NULL);
  int c5 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("drawCube time: %d\n",c5-c4);
  }
  
  drawGoal();

  if (timing) {
  gettimeofday(&tim, NULL);
  int c6 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("drawGoal time: %d\n",c6-c5);
  }

  // Print pause menu
  if (getGameplayRunning()) {
    //printString("RUNNING",playerX[player],playerY[player]+250,playerZ[player]);
  } else {
    int n, a=getLastPause() + 1;
    if (a > 0) {
      n=sprintf(pausedText, "P%d PAUSED", a);
    } else {
      n=sprintf(pausedText, "ALL PAUSED");
    }
    printString(pausedText,playerX[player],playerY[player]+200,playerZ[player]);
  }
  // And player stats (wip/temp)
  if (getPlayer(0)->getGrounded()) { printString("grounded",0,140,0); } else { printString("flying",0,120,0); }

  if (timing) {
  gettimeofday(&tim, NULL);
  int c7 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("drawFor nested time: %d\n",c7-c1);
  printf("&&&&&& DISPLAYFOR CUBIOR END &&&&&&&&\n");
  }
  
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

  int c1,c2,c3,c4,c5;
  if (timing) { gettimeofday(&tim, NULL); int c1 = (tim.tv_sec+(tim.tv_usec/1.0)); }

  glPushMatrix();
  // Move player
  glTranslatef(cubeX[n], cubeY[n], cubeZ[n]);
  
  // And make player bigger
  glScalef(100.0,100.0,100.0);

  if (timing) { gettimeofday(&tim, NULL); int c2 = (tim.tv_sec+(tim.tv_usec/1.0)); }

  cubeShape[n].draw();

  if (timing) { gettimeofday(&tim, NULL); int c3 = (tim.tv_sec+(tim.tv_usec/1.0)); }

  glPopMatrix();

  if (timing) { gettimeofday(&tim, NULL); int c4 = (tim.tv_sec+(tim.tv_usec/1.0)); }

  
if (timing && c4-c1 > 200) {
  printf(" %dth....drawCube number start....\n", n);
  printf("prep time: %d\n",c2-c1);
  printf("draw time: %d\n",c3-c2);
  printf("popMatrix time: %d\n",c4-c3);
  printf("drawCube nested time: %d\n",c4-c1);
  printf(".....drawCube end.....\n");
}
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
  setPerspective(1,1);
}

void setPerspective(int x, int y) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); // blank canvas for transforms
  GLfloat aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
  gluPerspective(45.0, aspect*x/y, 0.50, 10.0);
}

// main loop for rendering. Also calls gameplay loop,
// updates graphics, and calls itself again
// BASICALLY: main loop for everything
void renderLoop() {
  //time_t seconds;
  //seconds = time(NULL);

  int dTime1,dTime2,dTime3,dTime4,dTime5,dTime6;
  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime1 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("restOfGame time: %d\n",dTime1-lastTime1);
  lastTime1 = dTime1;
  }
  
  sendCommands();

  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime2 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("sendCommands() time: %d\n",dTime2-dTime1);
  }
  
  gameplayLoop();

  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime3 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("gameplayLoop() time: %d\n",dTime3-dTime2);
  }
  
  for (int i=0; i<cubiorNum; i++) {
    updatePlayerGraphic(i);
  }

  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime4 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("updatePlayerGraphic() time: %d\n",dTime4-dTime3);
  }
  
  for (int i=0; i<cubeNum; i++) {
    updateCubeGraphic(i);
  }

  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime5 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("updateCubeGraphic() time: %d\n",dTime5-dTime4);
  }
  
  updateGoalGraphic();

  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime6 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("updateGoalGraphic() time: %d\n",dTime6-dTime5);
  }
  
  glutPostRedisplay();

  if (timing) {
  gettimeofday(&tim, NULL);
  int dTime7 = (tim.tv_sec+(tim.tv_usec/1.0));
  printf("glutPostRedisplay() time: %d\n",dTime7-dTime6);
  printf("renderLoop() time: %d\n",dTime7-dTime1);
  }
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
    /* May not need this, updateCubeGraphic does the work
    cubeX[i] = 0.0;
    cubeY[i] = 0.0;
    cubeZ[i] = 0.0;*/
    cubeCollision[i] = false;
    updateCubeGraphic(i);
    int altSize = 400;
    bool alternatingSpot =(
        (cubeX[i]<0)^((int(abs(cubeX[i]+1))%(altSize*2)<altSize))
      ) ^ (
        (cubeY[i]<0)^((int(abs(cubeY[i]+1))%(altSize*2)<altSize))
      )^ (
        (cubeZ[i]<0)^((int(abs(cubeZ[i]+1))%(altSize*2)<altSize))
      );
    cubeShape[i].initVisuals(0.95,1.0,0.5,0.5,alternatingSpot,cubeY[i]<=getFloor()  && abs(cubeZ[i])!=mapWidth/2);
    cubeShape[i].setNeighbors(getCube(i)->getNeighbors());
  }

  // Initialize Goal Visual Vals
  goalX = 0.0;
  goalX = 0.0;
  goalX = 0.0;
  goalShape.initGoalVisuals();
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

  if (refreshOnIdle) {
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

void printString(char *string, int x, int y, int z) {
  int len, i;
  len = (int)strlen(string);
  glColor3f( 0.0f, 0.0f, 0.0f );
  glRasterPos3f(-15.0f*len+x,0.0f+y,0.0f+z);
  //glRasterPos3f(-4.5f*len+x,200.0f+y,590.0f);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
  }
}

bool getTiming() { return timing; }
