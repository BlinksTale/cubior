/*
 * flatRender.cpp
 * by Brian Handy
 * 1/20/12
 * 2d Visuals for cubior
 */
#include "flatRender.h"
#include "sfx.h"
#include "gameplay.h"
#include "keyboard.h"
#include "cubeShape.h"
#include "goalShape.h"
#include "cubiorShape.h"

#define _USE_MATH_DEFINES
#include <math.h> // for M_PI

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
//#include <sys/time.h> // for linux time

// Intended Frames Per Second do not change
static const int FPS = 60;
int windowWidth = 640;
int windowHeight = 480;
int oldWindowWidth = windowWidth;
int oldWindowHeight = windowHeight;
bool fullscreen = true;
bool drawOutlines = false;
// Whether to wait for idle to refresh, or force w/ timer
static const bool refreshOnIdle = false; // works better, otherwise hangs when PC busy

// Cubior and Cube Count vals (duplicates from Gameplay, will link them later)
const int cubiorNum = cubiorCount;
const int maxCubeNum = maxCubeCount;// 9 + playableWidth*playableDepth;
int cubeNum;
char pausedText[50];
bool levelShadows = true;

// angle of cubior while he rotates
static float playerAngleNumerator[cubiorNum];
static float playerAngleDivisor[cubiorNum];
const int maxPlayerRotations = 5;
int currentPlayerRotation[cubiorNum];
float playerRotationAngle[cubiorNum][maxPlayerRotations];

// pos of cubiors while they move
CubiorShape cubiorShape[cubiorNum];
static GLfloat playerX[cubiorNum];
static GLfloat playerY[cubiorNum];
static GLfloat playerZ[cubiorNum];
static GLfloat changeX[cubiorNum];
static GLfloat changeY[cubiorNum];
static GLfloat changeZ[cubiorNum];
static GLfloat lastChangeX[cubiorNum];
static GLfloat lastChangeZ[cubiorNum];
static GLfloat meanChangeX[cubiorNum];
static GLfloat meanChangeZ[cubiorNum];

// pos of cube obstacles
//static int cubesTotal = 1;
CubeShape cubeShape[maxCubeNum];
static GLfloat cubeX[maxCubeNum];
static GLfloat cubeY[maxCubeNum];
static GLfloat cubeZ[maxCubeNum];
static bool cubeCollision[maxCubeNum];

// time stuff
bool timing = false;
//struct timeval tim;
int lastTime1 = 0;
int lastTime2 = 0;
int lastTime3 = 0;
int lastTime4 = 0;
int lastTime5 = 0;
int lastClock = clock();
int averageDiff = 0;

// Goal object's visual
GoalShape goalShape;
static GLfloat goalX;
static GLfloat goalY;
static GLfloat goalZ;

// Pointers to oft referenced objects
CameraObj* cameraPointer[cubiorNum];

int getFPS() {
  int newClock = clock();
  int newDiff = newClock - lastClock;
  lastClock = newClock;
  averageDiff = averageDiff*0.9 + newDiff*0.1;
  return CLOCKS_PER_SEC/averageDiff; // wtf xD so lazy FIXME
}

// Display (name chosen from examples of Dr. Toal & Dr. Dionisio)
void display() {
  //cout << "FPS: " << getFPS() << endl;
  
  //int dTime1, dTime2, dTtime3, dTime4, dTime5, dTime6;
  /*
  if (timing) {
    printf("------------\n");
    gettimeofday(&tim, NULL);
    int dTime1 = (tim.tv_sec+(tim.tv_usec/1.0));
    lastTime2 = dTime1;
  }*/
  
  glScissor(0,0,windowWidth,windowHeight);
  glViewport(0,0,windowWidth,windowHeight);

  // Make sure background is black first, then draw on top of it
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // do not understand order, but this works
  
  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime2 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("preppingScreen time: %d\n",dTime2-dTime1);
  }*/
  
  // Draw all playing Cubior views
  for (int i=0; i<getCubiorCount(); i++) {
    
    if (getCubiorPlayable(i)) {
      /*int cT1, cT2, cT3, cT4, cT5;
      if (timing) {
      printf("=====CUBIOR DRAW BEGIN=======\n");
      gettimeofday(&tim, NULL);
      int cT1 = (tim.tv_sec+(tim.tv_usec/1.0));
      }*/
      
      int w=i%2;
      int h=i>1?1:0;
      int altX = (h)*2+1-w; // side screen neighbor
      int altY = (1-h)*2+w; // elevator screen neighbor
      int altXY = getCubiorCount()-i-1; // diagonal screen neighbor
      int posX = windowWidth*w/2 +(2*w-1); // screen position
      int posY = windowHeight*(1-h)/2+(2*(1-h)-1); // screen position
      int viewW = windowWidth*1/2;
      int viewH = windowHeight*1/2;

      /*if (timing) {
          gettimeofday(&tim, NULL);
          int cT2 = (tim.tv_sec+(tim.tv_usec/1.0));
          printf("prep Cubior time: %d\n",cT2-cT1);
      }*/
  
      // Now fill in empty space
      if (!getCubiorPlayable(altX)
        &&!getCubiorPlayable(altY)
        &&!getCubiorPlayable(altXY)) {

        // All alone? Fill all the screen space!
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

      /*if (timing) {
          gettimeofday(&tim, NULL);
          int cT3 = (tim.tv_sec+(tim.tv_usec/1.0));
          printf("splitscreen fill time: %d\n",cT3-cT2);
      }*/
  
      // Draw some player i
      glScissor(posX,posY,viewW,viewH);

      /*if (timing) {
          gettimeofday(&tim, NULL);
          int cT4 = (tim.tv_sec+(tim.tv_usec/1.0));
          printf("scissor time: %d\n",cT4-cT3);
      }*/
  
      glViewport(posX, posY, viewW, viewH);

      /*if (timing) {
          gettimeofday(&tim, NULL);
          int cT5 = (tim.tv_sec+(tim.tv_usec/1.0));
          printf("viewport time: %d\n",cT5-cT4);
      }*/
      displayFor(i);

      /*if (timing) {
          gettimeofday(&tim, NULL);
          int cT6 = (tim.tv_sec+(tim.tv_usec/1.0));
          printf("displayFor time: %d\n",cT6-cT5);
      }*/
      
    }
    
    /*if (timing) {
      printf("======CUBIOR DRAW END========\n");
    }*/
  }

  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime3 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("drawCubiors time: %d\n",dTime3-dTime2);
  }*/
  
  // End with a quick flush, to draw faster
  glFlush();
  glutSwapBuffers(); // because using double buffer, must swap buffers

  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime4 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("totalDrawTime() time: %d\n",dTime4-dTime1);
    printf("------------\n");
  }*/

}

// Draw an entire player's screen
void displayFor(int player) {
  
  // Paint background cyan to neon blue
  glClearColor(getMapRed(), getMapGreen(), getMapBlue(), 0.0f);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  // Zoom camera out,
  glScalef(0.001,0.001,0.001);
  // Set camera angle and pos, based off averages
  glRotatef(-1.0*cameraPointer[player]->getMeanAngleX(),1.0,0.0,0.0);
  glRotatef(-1.0*cameraPointer[player]->getMeanAngleY(),0.0,1.0,0.0);
  glRotatef(-1.0*cameraPointer[player]->getMeanAngleZ(),0.0,0.0,1.0);
  // then pull back and up to see cubes
  glTranslatef(-1.0*cameraPointer[player]->getMeanX(),-1.0*cameraPointer[player]->getMeanY(),-1.0*cameraPointer[player]->getMeanZ());
  
  // Only try block culling if looking from close to the ground
  if (cameraPointer[player]->getMeanY() < playerY[player]+2000) {
    int backwardsDist = 500;
    int camAngleNow = ((int)cameraPointer[player]->getMeanAngleY() + 720 + 180 - 45) % 360;
    int camFacingX = (camAngleNow < 180 || camAngleNow > 270)*(1)+(-1)*(camAngleNow > 90 || camAngleNow < 0);
    int camFacingZ = (camAngleNow > 270 && camAngleNow < 360)*(-1)+(1)*(camAngleNow < 180 && camAngleNow > 90);
    
    if (drawOutlines) {
      for (int i=0; i<cubeNum; i++) {
        // Only draw if it's in the range the camera will see
        int deltaX = cameraPointer[player]->getMeanX() - cubeX[i];
        int deltaZ = -cameraPointer[player]->getMeanZ() + cubeZ[i]; // Oddly, must be negated to work

        if ((deltaX*camFacingX<backwardsDist)
          &&(deltaZ*camFacingZ<backwardsDist)) {
          drawCubeOutline(i);
        }
      }
    }
    
    for (int i=0; i<cubeNum; i++) {
      // Only draw if it's in the range the camera will see
      int deltaX = cameraPointer[player]->getMeanX() - cubeX[i];
      int deltaZ = -cameraPointer[player]->getMeanZ() + cubeZ[i]; // Oddly, must be negated to work
      
      if ((deltaX*camFacingX<backwardsDist)
        &&(deltaZ*camFacingZ<backwardsDist)) {
        drawCube(i,player);
      }
    }
  // So if far away, just draw cubes normally
  } else {
    if (drawOutlines) {
      for (int i=0; i<cubeNum; i++) {
        drawCubeOutline(i);
      }
    }
    for (int i=0; i<cubeNum; i++) {
      drawCube(i,player);
    }
  }
  // Draw player as last thing before HUD
  for (int i=0; i<cubiorNum; i++) { calcPlayer(i); }
  if (drawOutlines) {
    for (int i=0; i<cubiorNum; i++) { drawPlayerOutline(i); }
  }
  for (int i=0; i<cubiorNum; i++) { drawPlayerSilhouette(i); }
  for (int i=0; i<cubiorNum; i++) { drawPlayer(i); }
  
  // I lied, draw goal last since likely higher than player,
  // and this shows shadow secondarily
  if (drawOutlines) { drawGoalOutline(); }
  drawGoal();
  
  // Then draw all shadows in order of height!
  drawAllShadows();
  
  // Print pause menu
  if (getGameplayRunning()) {
      int n, a=cameraPointer[player]->getMeanAngleX(); //truth
      // Commented out so that these values can easily be displayed again if necessary 
      // printString(pausedText,playerX[player],playerY[player]+200,playerZ[player]);
      // printString("RUNNING",playerX[player],playerY[player]+250,playerZ[player]);
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
  // Commented out so that these values can easily be displayed again if necessary 
  // if (getPlayer(0)->getGrounded()) { printString("grounded",0,140,0); } else { printString("flying",0,120,0); }
  
}

// Give shadows to everything!
void drawAllShadows() {
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glEnable(GL_STENCIL_TEST);
    
    for (int i=0; i<cubeNum; i++) { drawCubeShadow(i); }
    for (int i=0; i<cubiorNum; i++) { drawPlayerShadow(i); }
    drawGoalShadow();
    
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
    
		glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		fillScreenWithShadow();
		glDisable(GL_STENCIL_TEST);

    glDisable( GL_BLEND );
    // end of shadow stuff
}

// 100% copied from draw_shadow for testing purposes
// full permission to copy this though, according to Josh Beam:
// http://joshbeam.com/articles/stenciled_shadow_volumes_in_opengl/
void fillScreenWithShadow() {
  glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);

	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, 1);
		glVertex2i(1, 1);
		glVertex2i(1, 0);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

// Figure out new stats for drawings before doing any drawings
void calcPlayer(int n) {
  if (getCubiorPlayable(n)) {
    // Then animate player rotation
    // new way
	//if (sqrt(pow((float)(changeX[n]),2) + pow((float)(changeZ[n]),2)) >= sqrt(pow((float)(lastChangeX[n]),2) + pow((float)(lastChangeZ[n]),2))) {
	  // old way
	float changeMin = 5.0;
	// Only an issue for changeZ = -20 to changeZ = 0. Not sure why.
    if (meanChangeX[n] > changeMin || meanChangeX[n] < -changeMin || meanChangeZ[n] > changeMin || meanChangeZ[n] < -changeMin) {
      playerAngleDivisor[n] = ((meanChangeZ[n] != 0.0) ? meanChangeZ[n] : 0.01);
      playerAngleNumerator[n] = meanChangeX[n];
    }
    lastChangeZ[n] = meanChangeZ[n];
  //cout << "numerat is " << playerAngleNumerator[n] << endl;
  //cout << "divisor is " << playerAngleDivisor[n] << endl;
  }
}

void playerPreDraw(int n) {
  glPushMatrix();
  // Move player
  glTranslatef(playerX[n], playerY[n], playerZ[n]);
  // Find new angle
  float oldAngle = playerRotationMean(n);
  float newAngle = oldAngle;
  if (lastChangeZ[n] < 0.0) {
	newAngle = atan(playerAngleNumerator[n]/playerAngleDivisor[n])*60.0 + 180;
  } else {
    newAngle = atan(playerAngleNumerator[n]/playerAngleDivisor[n])*60.0;
  }
  // Then modify it to match current mean
  while (newAngle > oldAngle + 180) { newAngle -= 2*180; }
  while (newAngle < oldAngle - 180) { newAngle += 2*180; }
  playerRotationAngle[n][currentPlayerRotation[n]] = newAngle;
  // Then move to next slot
  currentPlayerRotation[n]++;
  if (currentPlayerRotation[n] >= maxPlayerRotations) {
	  currentPlayerRotation[n] = 0;
  }
  // Finally, apply rotation
  float avg = playerRotationMean(n);
  //cout << "newAngle is " << newAngle << endl;
  //cout << "avg is " << avg << endl;
  glRotatef(newAngle,0.0,1.0,0.0);
  
  // And make player bigger
  glScalef(100.0,100.0,100.0);
}

// like angle mean, puts together all 
float playerRotationMean(int n) {
	float sumOfAngles = 0;
	for (int i=0; i<maxPlayerRotations; i++) {
		sumOfAngles += playerRotationAngle[n][i];
	}
	return (sumOfAngles*1.0/maxPlayerRotations);
}

void playerPostDraw(int n) {
  glPopMatrix();
}
  
void drawPlayer(int n) {
  if (getCubiorPlayable(n)) {
    playerPreDraw(n);
    cubiorShape[n].drawCubior(n);
    playerPostDraw(n);
  }
}

void drawPlayerSilhouette(int n) {
  if (getCubiorPlayable(n)) {
    playerPreDraw(n);
    cubiorShape[n].drawCubiorSilhouette();
    playerPostDraw(n);
  }
}

// Same as drawPlayer, but shadows separately
void drawPlayerShadow(int n) {
  if (getCubiorPlayable(n)) {
    playerPreDraw(n);
    cubiorShape[n].drawShadow();
    playerPostDraw(n);
  }
}


// Same as drawPlayer, but shadows separately
void drawPlayerOutline(int n) {
  if (getCubiorPlayable(n)) {
    playerPreDraw(n);
    cubiorShape[n].drawOutline();
    playerPostDraw(n);
  }
}

void drawCube(int n, int player) {
  // ALWAYS start by setting abovecam stat
  cubeShape[n].setAboveCam(cubeY[n]-cameraPointer[player]->getMeanY()>0);
          
  int c1,c2,c3,c4,c5;
  //if (timing) { gettimeofday(&tim, NULL); int c1 = (tim.tv_sec+(tim.tv_usec/1.0)); }

  glPushMatrix();
  // Move player
  glTranslatef(cubeX[n], cubeY[n], cubeZ[n]);
  
  // And make player bigger
  glScalef(100.0,100.0,100.0);

  //if (timing) { gettimeofday(&tim, NULL); int c2 = (tim.tv_sec+(tim.tv_usec/1.0)); }

  //cubeShape[n].drawShadow();
  cubeShape[n].draw();

  //if (timing) { gettimeofday(&tim, NULL); int c3 = (tim.tv_sec+(tim.tv_usec/1.0)); }

  glPopMatrix();

  //if (timing) { gettimeofday(&tim, NULL); int c4 = (tim.tv_sec+(tim.tv_usec/1.0)); }

  
  /*if (timing && c4-c1 > 200) {
    printf(" %dth....drawCube number start....\n", n);
    printf("prep time: %d\n",c2-c1);
    printf("draw time: %d\n",c3-c2);
    printf("popMatrix time: %d\n",c4-c3);
    printf("drawCube nested time: %d\n",c4-c1);
    printf(".....drawCube end.....\n");
  }*/
}

// Same as drawCube, but shadows separately
void drawCubeShadow(int n) {
  if (cubeShape[n].hasShadow() && levelShadows) {
    glPushMatrix();
    glTranslatef(cubeX[n], cubeY[n], cubeZ[n]);
    glScalef(100.0,100.0,100.0);
    cubeShape[n].drawShadow();
    glPopMatrix();
  }
}

// Same as drawCube, but just an outline
void drawCubeOutline(int n) {
  glPushMatrix();
  glTranslatef(cubeX[n], cubeY[n], cubeZ[n]);
  glScalef(100.0,100.0,100.0);
  cubeShape[n].drawOutline();
  glPopMatrix();
}

void drawGoal() {
  glPushMatrix();
  // Move player
  glTranslatef(goalX, goalY, goalZ);
  
  // And make player bigger
  glScalef(100.0,100.0,100.0);
  goalShape.drawGoal();
  //goalShape.drawShadow();
  glPopMatrix();
}

// Same as drawGoal, but shadows separately
void drawGoalShadow() {
  glPushMatrix();
  glTranslatef(goalX, goalY, goalZ);
  glScalef(100.0,100.0,100.0);
  goalShape.drawShadow();
  glPopMatrix();
}

// Same as drawGoal, but shadows separately
void drawGoalOutline() {
  glPushMatrix();
  glTranslatef(goalX, goalY, goalZ);
  glScalef(100.0,100.0,100.0);
  goalShape.drawOutline();
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
  gluPerspective(45.0, aspect*x/y, 0.050, 100.0);
}

// main loop for rendering. Also calls gameplay loop,
// updates graphics, and calls itself again
// BASICALLY: main loop for everything
void renderLoop() {
  //time_t seconds;
  //seconds = time(NULL);

  // Variables to track time spent on each section
  // for lag info
  int dTime1,dTime2,dTime3,dTime4,dTime5,dTime6;

  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime1 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("restOfGame time: %d\n",dTime1-lastTime1);
    lastTime1 = dTime1;
  }*/
  
  sendCommands();

  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime2 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("sendCommands() time: %d\n",dTime2-dTime1);
  }*/
  
  gameplayLoop();
  
  sfxLoop();

  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime3 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("gameplayLoop() time: %d\n",dTime3-dTime2);
  }*/
  
  for (int i=0; i<cubiorNum; i++) {
    updatePlayerGraphic(i);
  }

  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime4 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("updatePlayerGraphic() time: %d\n",dTime4-dTime3);
  }*/
  
  // FIXME: Wait, we should totally not need to update this every loop
  // the cubes are all static. Leave them that way.
  /*for (int i=0; i<cubeNum; i++) {
    updateCubeGraphic(i);
  }*/

  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime5 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("updateCubeGraphic() time: %d\n",dTime5-dTime4);
  }*/
  
  updateGoalGraphic();
  
  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime6 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("updateGoalGraphic() time: %d\n",dTime6-dTime5);
  }*/
  
  glutPostRedisplay();

  /*if (timing) {
    gettimeofday(&tim, NULL);
    int dTime7 = (tim.tv_sec+(tim.tv_usec/1.0));
    printf("glutPostRedisplay() time: %d\n",dTime7-dTime6);
    printf("renderLoop() time: %d\n",dTime7-dTime1);
  }*/
}

void timerRenderLoop(int v) {
  renderLoop();
  glutTimerFunc(1000/FPS, timerRenderLoop, v);
}

// To setup the items we will show
void initVisuals() {

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

  // Get the Cube count
  cubeNum = getCubeCount();

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
    // Yellow case
    if (getCube(i)->getMaterial()==9) {
      cubeShape[i].initVisuals(0.95,1.00,0.50, 0.9,1.0,0.5, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
    // Snow case
    } else if (getCube(i)->getMaterial()==3) {
      cubeShape[i].initVisuals(0.87,0.87,1.00, 1.0,1.0,1.0, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
    // Stone case
    } else if (getCube(i)->getMaterial()==2) {
      cubeShape[i].initVisuals(0.52,0.62,0.54, 0.9,0.5,0.5, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
    // Grass case
    } else {
      cubeShape[i].initVisuals(0.92,0.62,0.04, 0.0,0.9,0.0, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
    }
    cubeShape[i].setNeighbors(getCube(i)->getNeighbors());
    cubeShape[i].setShadow(getShadow(i));
  }

  // Initialize Goal Visual Vals
  goalX = 0.0;
  goalX = 0.0;
  goalX = 0.0;
  goalShape.setGlow(false);
  goalShape.initGoalVisuals();
  updateGoalGraphic();

}

// To setup OpenGL and GLUT in general
void initFlat(int argc, char** argv) {

  // Setup all-cubes-of-any-form visuals 
  initVisuals();

  // Then do the same for sound effects
  initSfx(argc, argv);
  
  // standard initialization
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL);
    
  // setup & create window
  glutInitWindowPosition(0,0);
  glutInitWindowSize(windowWidth,windowHeight);
  glutCreateWindow("Cubior");
  
  // Lets me apply fullscreen val as starting state
  // without splitting up function :P Might fix later.
  fullscreen = !fullscreen;
  toggleFullscreen();
  
  // Make sure back faces are behind front faces
  glEnable(GL_DEPTH_TEST); // And GL_DEPTH_BUFFER_BIT is in clear calls too!
  glDepthFunc(GL_LEQUAL); // determines what is chosen as visible, that with a less than/eq dist from cam
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK); // This is odd, it should be GL_BACK. Camera must be inverted or something
  glEnable(GL_SCISSOR_TEST); // For splitscreen, must come after Window is created/Init'd

  // input
  glutKeyboardFunc(inputDown);
  glutKeyboardUpFunc(inputUp);
  glutSpecialFunc(specialInputDown);
  glutSpecialUpFunc(specialInputUp);
  // no need for this limited function,
  // SFML handles up to eight joysticks
  //glutJoystickFunc(joystickDown, 300);
  //glutForceJoystickFunc(); // makes joystick button presses get recognized

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
  if (( getGoal()->getGlow() && !goalShape.getGlow()) ||
      (!getGoal()->getGlow() &&  goalShape.getGlow())) { 
    goalShape.setGlow(getGoal()->getGlow()); 
  }
  
  setGoalGraphic(getGoal()->getX(),getGoal()->getY(),getGoal()->getZ());
}

void setPlayerGraphic(int n, int x, int y, int z) {
  if (abs(x-playerX[n]) > 2) {
    changeX[n] = x - playerX[n];
    playerX[n] = x;
  } else if (changeX[n] != 0) {
	  changeX[n] = 0;
  }
  if (abs(y-playerY[n]) > 2) {
    changeY[n] = y - playerY[n];
    playerY[n] = y;
  } else if (changeY[n] != 0) {
	  changeY[n] = 0;
  }
  if (abs(z-playerZ[n]) > 2) {
    changeZ[n] = z - playerZ[n];
    playerZ[n] = z;
  } else if (abs(changeZ[n]) > 0.001) {
	  if (changeZ[n] > 0.0) {
        changeZ[n] = 0.001;
	  } else {
	    changeZ[n] = -0.001;
	  }
  }
  // Finally, update the means
  float meanChangeRatio = 0.75;
  meanChangeX[n] = meanChangeRatio*meanChangeX[n] + (1.0-meanChangeRatio)*changeX[n];
  meanChangeZ[n] = meanChangeRatio*meanChangeZ[n] + (1.0-meanChangeRatio)*changeZ[n];
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

// Switch on/off fullscreen mode (from/to windowed)
void toggleFullscreen() {
  fullscreen = !fullscreen;
  
  if (fullscreen) {
	  oldWindowWidth = windowWidth;
    oldWindowHeight = windowHeight;
    glutFullScreen();
    glutSetCursor(GLUT_CURSOR_NONE);
  } else {
    glutReshapeWindow(oldWindowWidth,oldWindowHeight);
    glutSetCursor(GLUT_CURSOR_INHERIT);
  }
}

// Switch on/off fullscreen mode (from/to windowed)
void toggleLevelShadows() {
  //levelShadows = !levelShadows;
  // HIJACKED FOR OUTLINES! MUAHAHAA
  drawOutlines = !drawOutlines;
}
