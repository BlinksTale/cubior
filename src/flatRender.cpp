/*
 * flatRender.cpp
 * by Brian Handy
 * 1/20/12
 * 2d Visuals for cubior
 */
#include "flatRender.h"
#include "sfx.h"
#include "music.h"
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

// Starting values that change often in testing
bool printFPS = true;
bool fullscreen = false;
bool drawOutlines = false;
bool levelShadows = true; // at this point, shadows do not influence lag

// Intended Frames Per Second do not change
static const int FPS = 60;
int lastFPS = FPS;
int windowWidth = 640;
int windowHeight = 480;
int oldWindowWidth = windowWidth;
int oldWindowHeight = windowHeight;
// Whether to wait for idle to refresh, or force w/ timer
static const bool refreshOnIdle = false; // works better, otherwise hangs when PC busy

// Cubior and Cube Count vals (duplicates from Gameplay, will link them later)
const int cubiorNum = cubiorCount;
const int maxCubeNum = maxVisualCubeCount;// 9 + playableWidth*playableDepth;
int cubeNum;
char pausedText[50];

// Map drawing stuff for faster drawing
const int cubesCovered = 11000;//22000;//11000;//maxCubeCount;
GLuint betterIndices[36*cubesCovered];

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
static GLfloat playerDirection[cubiorNum];

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
int lastFrame = lastClock;
int averageDiff = 0;

// Goal object's visual
GoalShape goalShape;
static GLfloat goalX;
static GLfloat goalY;
static GLfloat goalZ;

// All vertices and indices for permanent cube shapes
int cubesVisible = 0; // how many cubes we'll reference
int facesVisible = 0; // how many cube faces we'll actually draw
int topFacesVisible = 0; // how many cube top faces we'll draw
int shadowsVisible = 0; // how many shadows to draw

// merger of all unmoving cube shapes to fit in one draw call
GLuint superIndices[maxCubeCount*24];
GLfloat superVertices[maxCubeCount*24];
GLfloat superColors[maxCubeCount*24];

// Give top of tiles a different color
GLuint topIndices[maxCubeCount*24];
GLfloat topVertices[maxCubeCount*24];
GLfloat topColors[maxCubeCount*24];

// Now merge the two together for one giant amazing draw call!
GLuint ultimateIndices[maxCubeCount*24*2];
GLfloat ultimateVertices[maxCubeCount*24*2];
GLfloat ultimateColors[maxCubeCount*24*2];

// Finally, an attempt to share this with the shadow world
GLuint shadowIndices[maxCubeCount*24];
GLfloat shadowVertices[maxCubeCount*24];
GLfloat shadowColors[maxCubeCount*24];

// Pointers to oft referenced objects
CameraObj* cameraPointer[cubiorNum];

int getFPS() {
  int newClock = clock();
  int newDiff = newClock - lastFrame;
  lastFrame = newClock;
  averageDiff = averageDiff*0.9 + newDiff*0.1;
  lastFPS = CLOCKS_PER_SEC/averageDiff; // wtf xD so lazy FIXME
  return lastFPS;
}
int getLastFPS() {
  return lastFPS;
}

// Time since last called
int getTimePassed() {
  int newClock = clock();
  int newDiff = newClock - lastClock;
  lastClock = newClock;
  return newDiff;
}

// Display (name chosen from examples of Dr. Toal & Dr. Dionisio)
void display() {
  if (printFPS) { cout << "FPS: \t" << getFPS() << endl; }
  //cout << "Since last frame ended: " << getTimePassed() << endl;
  
  glScissor(0,0,windowWidth,windowHeight);
  glViewport(0,0,windowWidth,windowHeight);

  // Make sure background is black first, then draw on top of it
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // do not understand order, but this works
  
  
  int cubiorsPlayable = getCubiorsPlayable();
  
  //cout << "Before the loop calls:  " << getTimePassed() << endl;
  // Draw all playing Cubior views
  for (int i=0; i<getCubiorCount(); i++) {
    
    if (getCubiorPlayable(i)) {
      //cout << "To setup player " << i << ": \t" << getTimePassed() << endl;
      
      int w=i%2;
      int h=i>1?1:0;
      int altX = (h)*2+1-w; // side screen neighbor
      int altY = (1-h)*2+w; // elevator screen neighbor
      int altXY = getCubiorCount()-i-1; // diagonal screen neighbor
      int posX = windowWidth*w/2 +(2*w-1); // screen position
      int posY = windowHeight*(1-h)/2+(2*(1-h)-1); // screen position
      int viewW = windowWidth*1/2;
      int viewH = windowHeight*1/2;
  
      //cout << "To init player " << i << ": \t" << getTimePassed() << endl;

      // Now fill in empty space
      //if (!getCubiorPlayable(altX)
      //  &&!getCubiorPlayable(altY)
      //  &&!getCubiorPlayable(altXY)) {
      if (cubiorsPlayable == 1) {
        // All alone? Fill all the screen space!
        viewW *= 2;
        viewH *= 2;
        posX = 0;
        posY = 0;
        setPerspective(2,2);
      } else if (cubiorsPlayable == 4) {
        // Quick exit for 4player mode, run faster!
        setPerspective(1,1);
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
      //cout << "To fill space " << i << ": \t" << getTimePassed() << endl;

      // Draw some player i
      glScissor(posX,posY,viewW,viewH);
      //cout << "To scissor for " << i << ": \t" << getTimePassed() << endl;
      
      glViewport(posX, posY, viewW, viewH); // was 12 lag compared to normal 1 for player 0?
      // might need some efficiency cleanup, FIXME
      //cout << "To viewport for " << i << ": \t" << getTimePassed() << endl;
      //cout << "All before disp " << i << ": \t" << getTimePassed() << endl;

      displayFor(i);
      //cout << "To display for " << i << ": \t" << getTimePassed() << endl;
      
    }
  }
  
  //cout << "Player loop ended: \t" << getTimePassed() << endl;

  // End with a quick flush, to draw faster
  // EDIT: No need for flush! This is only for when graphics card is on other side of the networks
  //glFlush();
  //cout << "Flush: \t\t\t" << getTimePassed() << endl;
  glutSwapBuffers(); // because using double buffer, must swap buffers
  //cout << "Swap: \t\t\t" << getTimePassed() << endl;
  //cout << "End: \t\t\t" << getTimePassed() << endl;

}

// Draw an entire player's screen
void displayFor(int player) {
  //cout << "Display for " << player << ":    \t\t" << getTimePassed() << endl;
  
  // Paint background cyan to neon blue
  glClearColor(getMapRed(), getMapGreen(), getMapBlue(), 0.0f); // do this here to allow black borders
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW); // must set this here to undo silhouette's effects later
  glLoadIdentity();
  
  // Zoom camera out,
  glScalef(0.001,0.001,0.001);
  // Set camera angle and pos, based off averages
  glRotatef(-1.0*cameraPointer[player]->getMeanAngleX(),1.0,0.0,0.0);
  glRotatef(-1.0*cameraPointer[player]->getMeanAngleY(),0.0,1.0,0.0);
  glRotatef(-1.0*cameraPointer[player]->getMeanAngleZ(),0.0,0.0,1.0);
  // then pull back and up to see cubes
  glTranslatef(-1.0*cameraPointer[player]->getMeanX(),-1.0*cameraPointer[player]->getMeanY(),-1.0*cameraPointer[player]->getMeanZ());
  
  //cout << "Starting half " << player << ":  \t\t" << getTimePassed() << endl;
  drawAllCubes(player);

  //cout << "Middle half " << player << ":  \t\t" << getTimePassed() << endl;
  // Draw player as last thing before HUD
  for (int i=0; i<cubiorNum; i++) { calcPlayer(i); }
  if (drawOutlines) {
    for (int i=0; i<cubiorNum; i++) { drawPlayerOutline(i); }
  }
  for (int i=0; i<cubiorNum; i++) { drawPlayerSilhouette(i); }
    // These code blocks modified from work on songho.ca
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
  for (int i=0; i<cubiorNum; i++) { drawPlayer(i); }
    // deactivate vertex arrays after drawing
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
  
  //cout << "Last half " << player << ":     \t\t" << getTimePassed() << endl;
  // I lied, draw goal last since likely higher than player,
  // and this shows shadow secondarily
  if (drawOutlines) { drawGoalOutline(); }
  drawGoal();
  
  // Then draw all shadows in order of height!
  drawAllShadows(player);
  
  // Print fullscreen FPS
  if (printFPS && fullscreen) {
    int n;
      n=sprintf(pausedText, "FPS %d", getLastFPS());
    printString(pausedText,playerX[player],playerY[player]+200,playerZ[player]);
    int d;
      d=sprintf(pausedText, "Rate %f", getFPSRate());
    printString(pausedText,playerX[player],playerY[player]+100,playerZ[player]);
  }
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
  
  //cout << "Ending half " << player << ":  \t\t" << getTimePassed() << endl;
}

// Give shadows to everything!
void drawAllShadows(int player) {
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glEnable(GL_STENCIL_TEST);
    
    if (levelShadows) {
    
      glEnableClientState(GL_COLOR_ARRAY);
      glEnableClientState(GL_VERTEX_ARRAY);
      
      // specify pointer to vertex array
      glColorPointer(3, GL_FLOAT, 0, shadowColors);
      glVertexPointer(3, GL_FLOAT, 0, shadowVertices);
      
      glPushMatrix();
      //glScalef(0.99,100.0,0.99);
      //glTranslatef(0.0,-50.505,0.0);
      
      glCullFace(GL_FRONT);
      glStencilFunc(GL_ALWAYS, 0x0, 0xff);
      glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
      glDrawElements(GL_TRIANGLES, 36*shadowsVisible, GL_UNSIGNED_INT, shadowIndices);

      glCullFace(GL_BACK);
      glStencilFunc(GL_ALWAYS, 0x0, 0xff);
      glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
      glDrawElements(GL_TRIANGLES, 36*shadowsVisible, GL_UNSIGNED_INT, shadowIndices);
      
      glPopMatrix();

      // deactivate vertex arrays after drawing
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
    
    }

    /*
    for (int i=0; i<cubeNum; i++) {
      // cubeWithinPlayerRange demonstrates noticeable improvements,
      // castle level with 4player, sfx and music went from 7fps to 12.
      // now goes from 24 to 35fps! Wow!
      if (cubeWithinPlayerRange(i,player)) {
        drawCubeShadow(i); 
      }
    }*/

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
  
    // FINALLY, update player angle, but only once!
    // Find new angle
    float oldAngle = playerRotationMean(n);
    float newAngle = oldAngle;
    // Only if an increase or equal amount of change do we alter direction
    /*if (abs(changeZ[n]-lastChangeZ[n])>=0 || abs(changeX[n]-lastChangeX[n])>=0) {
      if (lastChangeZ[n] < 0.0) {
	    newAngle = atan(playerAngleNumerator[n]/playerAngleDivisor[n])*60.0 + 180;
      } else {
        newAngle = atan(playerAngleNumerator[n]/playerAngleDivisor[n])*60.0;
      }
      // Then modify it to match current mean
      while (newAngle > oldAngle + 180) { newAngle -= 2*180; }
      while (newAngle < oldAngle - 180) { newAngle += 2*180; }
    }*/
    // OLD WAY ABOVE
    // NEW WAY BELOW
    if (newAngle != playerDirection[n]) {
      newAngle = playerDirection[n];
      // Then modify it to match current mean
      while (newAngle > oldAngle + 180) { newAngle -= 2*180; }
      while (newAngle < oldAngle - 180) { newAngle += 2*180; }
    }
    playerRotationAngle[n][currentPlayerRotation[n]] = newAngle;
    // Then move to next slot
    currentPlayerRotation[n]++;
    if (currentPlayerRotation[n] >= maxPlayerRotations) {
	    currentPlayerRotation[n] = 0;
    }
  }
}

void playerPreDraw(int n) {
  glPushMatrix();
  // Move player
  glTranslatef(playerX[n], playerY[n], playerZ[n]);
  
  // Finally, apply rotation
  float avg = playerRotationMean(n);
  //cout << "newAngle is " << newAngle << endl;
  //cout << "avg is " << avg << endl;
  glRotatef(avg,0.0,1.0,0.0);
  
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

// Call drawCube for as many times as there are cubes
void drawAllCubes(int player) {
  
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_INDEX_ARRAY);
    
    // specify pointer to vertex array
    glColorPointer(3, GL_FLOAT, 0, superColors);
    glVertexPointer(3, GL_FLOAT, 0, superVertices);
    glDrawElements(GL_TRIANGLES, 6*facesVisible, GL_UNSIGNED_INT, superIndices);
    
    glColorPointer(3, GL_FLOAT, 0, topColors);
    glVertexPointer(3, GL_FLOAT, 0, topVertices);
    glDrawElements(GL_TRIANGLES, 6*topFacesVisible, GL_UNSIGNED_INT, topIndices);
    
    /*glColorPointer(3, GL_FLOAT, 0, ultimateColors);
    glVertexPointer(3, GL_FLOAT, 0, ultimateVertices);
    glDrawElements(GL_TRIANGLES, 6*facesVisible+6*topFacesVisible, GL_UNSIGNED_INT, ultimateIndices);
    *///supposed to work version glDrawElements(GL_TRIANGLES, 6*topFacesVisible, GL_UNSIGNED_INT, topIndices);

    // Yeah, this one still has problems, even though some forum member recommended it more
    // http://www.gamedev.net/topic/562194-gldrawelementsarrays-crash/
    //glIndexPointer(GL_UNSIGNED_INT, 0, superIndices);
    //glDrawArrays(GL_TRIANGLES, 0, 36*cubesCovered);
    //glDisableClientState(GL_INDEX_ARRAY);
  
    // deactivate vertex arrays after drawing
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void drawCube(int n, int player) {
  // ALWAYS start by setting relations to cam
  // lets you skip drawing faces on opposite side from you
  /*cubeShape[n].setRelationToCam(
    cubeX[n]-cameraPointer[player]->getMeanX()>0,
    cubeY[n]-cameraPointer[player]->getMeanY()>0,
    cubeZ[n]-cameraPointer[player]->getMeanZ()>0
    );*/
  cubeShape[n].draw();
}

// Gives whether cube is close enough to draw shadow
bool cubeWithinPlayerRange(int n, int player) {
  int playerRange = 3000;
  // This is getting higher lag reports on AMD Code Analyst,
  // but not sure how to improve on it.
  int deltaX = abs(cubeX[n] - playerX[player]);
  int deltaY = abs(cubeY[n] - playerY[player]);
  int deltaZ = abs(cubeZ[n] - playerZ[player]);
  bool outtaX = (deltaX < playerRange);
  bool outtaY = (deltaY < playerRange);
  bool outtaZ = (deltaZ < playerRange);
  return (outtaX && outtaY && outtaZ);
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
  
  musicLoop();

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
    playerDirection[i] = 0.0;
    lastChangeZ[i] = 0.0;
    cubiorShape[i].initCubiorVisuals(i);
    updatePlayerGraphic(i);
  }

  // Get the Cube count
  cubeNum = getCubeCount();
  
  //Reset!
  cubesVisible = 0;
  facesVisible = 0;
  topFacesVisible = 0;
  shadowsVisible = 0;

  // Initialize Cube Visual Vals
  for (int i=0; i<cubeNum; i++) {
    // Only draw visible cubes
    if (!getCube(i)->isInvisible()) {
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
      // Polar case
      if (getCube(i)->getMaterial()==8) {
        cubeShape[i].initVisuals(1.0,1.00,1.0, 0.0,0.0,0.0, 1.0,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      }
      // Cave case
      } else if (getCube(i)->getMaterial()==7) {
        cubeShape[i].initVisuals(0.10,0.15,0.65, 0.20,0.10,0.55,0.25,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Bridge case
      } else if (getCube(i)->getMaterial()==6) {
        cubeShape[i].initVisuals(0.80,0.52,0.25, 0.90,0.62,0.35, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Canyon case
      } else if (getCube(i)->getMaterial()==5) {
        cubeShape[i].initVisuals(0.90,0.35,0.11, 1.0,0.58,0.41, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Rock case
      } else if (getCube(i)->getMaterial()==4) {
        cubeShape[i].initVisuals(0.4,0.4,0.4, 0.5,0.5,0.5, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Snow case
      } else if (getCube(i)->getMaterial()==3) {
        cubeShape[i].initVisuals(0.87,0.87,1.00, 1.0,1.0,1.0, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Castle Wall case
      } else if (getCube(i)->getMaterial()==2) {
        cubeShape[i].initVisuals(0.52,0.62,0.54, 0.9,0.5,0.5, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Grass case
      } else {
        cubeShape[i].initVisuals(0.92,0.62,0.04, 0.0,0.9,0.0, 0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      }
      cubeShape[i].setNeighbors(getCube(i)->getVisibleNeighbors());
      cubeShape[i].permanentPosition(cubeX[i], cubeY[i], cubeZ[i]);
      cubeShape[i].setShadow(getShadow(i));
    
      // If even one face is visible, include the cube's vertices
      if (cubeShape[i].hasVisibleFace()) {
        //cout << "Now on cube " << i << " with cubesVisible = " << cubesVisible << endl;
        // OK, add the cube's vertices. There are 988 cubes, so 988*8 vertices/cube = 7904 vertices total, or 23712 values to make them
        for (int vertex=0; vertex<24; vertex++) {
          //if (vertex<18 && vertex>11) { topVertices[cubesVisible*6+vertex] = cubeShape[i].getVertex(vertex); }
          //if (vertex<18 && vertex>11) { topColors[cubesVisible*6+vertex] = cubeShape[i].getColor(vertex); }
          superVertices[cubesVisible*24+vertex] = cubeShape[i].getVertex(vertex);
          superColors[cubesVisible*24+vertex] = cubeShape[i].getColor(vertex);
        }
        // Alright, now find that face. There are 36 indices per cube, so 36*988 cubes = 35568 indices total
        for (int face=0; face<6; face++) {
          if (cubeShape[i].hasFace(face) && face != 2) {
            // Add all 6 indices for that face
            for (int vertex=0; vertex<6; vertex++) {
              // in the index for that face + that vertex, put the index from that face and vertex for that cube
              // the 8 at the end is since we are using all 8 vertices every time
              superIndices[facesVisible*6+vertex] = cubeShape[i].getIndex(face*6+vertex) + cubesVisible*8;
            }
            // And remember that we've added a face
            facesVisible++;
          }
        }
        // And remember the cube we added too
        cubesVisible++;
      }


      // temp set of indices for only top faces
      GLuint tempTopIndices[]  = { // counterclockwise draws forward
                         // Top
                         2, 1, 0, // upper front left
                         2, 3, 1, // upper rear right
                       }; 

      // Now do the same for top faces
      if (cubeShape[i].hasFace(2)) { // top face exists?
        // Get vertices // VERTICES ARE... MOSTLY WORKING
        for (int vertex=0; vertex<12; vertex++) {
          // top vertices live in cupeShape's 0-5 and 12-17 vertices, so add 6 to vertex for second half
          topVertices[topFacesVisible*12+vertex] = cubeShape[i].getVertex(vertex + (vertex < 6 ? 0 : 6));
          //cout << "Now topVertices[" << topFacesVisible*12+vertex << "] holds " << topVertices[topFacesVisible*12+vertex] << endl;
        }
        // Get colors // COLORS ARE WORKING
        for (int color=0; color<12; color++) {
          // matching colors are all called with getTopColor
          topColors[topFacesVisible*12+color] = cubeShape[i].getTopColor(color);
          //cout << "Now topColors[" << topFacesVisible*12+color << "] holds " << topColors[color] << endl;
        }
        // Get indices // INDICES ARE... NOT ALWAYS WORKING?
        for (int vertex=0; vertex<6; vertex++) {
          // the 4 at the end is because we only use four vertices total for every face
          topIndices[topFacesVisible*6+vertex] = tempTopIndices[vertex] + topFacesVisible*4;
          //cout << "Now topIndices[" << topFacesVisible*6+vertex << "] holds " << topIndices[topFacesVisible*6+vertex] << endl;
        }
        topFacesVisible++;
      }

      // Finally, if it has a shadow, remember that too
      if (cubeShape[i].hasShadow()) {
        // OK, add the cube's vertices. There are 988 cubes, so 988*8 vertices/cube = 7904 vertices total, or 23712 values to make them
        for (int vertex=0; vertex<24; vertex++) {
          shadowVertices[shadowsVisible*24+vertex] = cubeShape[i].getShadowVertex(vertex); // apply scale (*) then transforms (+) here later
          shadowColors[shadowsVisible*24+vertex] = 0; // No color! It's a shadow! :D
        }
        // Now add all indices, 36 per cube (6 per face, 6 faces
        for (int vertex=0; vertex<36; vertex++) {
            // in the index for that face + that vertex, put the index from that face and vertex for that cube
            // the 8 at the end is since we are using all 8 vertices every time
            shadowIndices[shadowsVisible*36+vertex] = cubeShape[i].getIndex(vertex) + shadowsVisible*8; // only 8 vertices/cube!
        }
        // And remember the cube we added too
        shadowsVisible++;
      }
    } // end of visible cubes
  }

  // Now transfer everything!
  // First, normal vertices and colors
  for (int i=0; i<cubesVisible; i++) {
    for (int vertex=0; vertex<24; vertex++) {
      ultimateVertices[i*24+vertex] = superVertices[i*24+vertex];
      ultimateColors[i*24+vertex] = superColors[i*24+vertex];
    }
  }
  for (int i=0; i<facesVisible; i++) {
    for (int vertex=0; vertex<6; vertex++) {
      // in the index for that face + that vertex, put the index from that face and vertex for that cube
      ultimateIndices[i*6+vertex] = superIndices[i*6+vertex];
    }
  }
  // And add top vertices, colors and faces
  for (int i=0; i<topFacesVisible; i++) {
    for (int vertex=0; vertex<12; vertex++) {
      // top vertices live in cupeShape's 0-5 and 12-17 vertices, so add 6 to vertex for second half
      ultimateVertices[cubesVisible*24+i*12+vertex] = topVertices[i*12+vertex];
      ultimateColors[cubesVisible*24+i*12+vertex] = topColors[i*12+vertex];
    }
    for (int vertex=0; vertex<6; vertex++) {
      // the 4 at the end is because we only use four vertices total for every face
      ultimateIndices[facesVisible*6+i*6+vertex] = topIndices[i*6+vertex] + cubesVisible*8;
    }
  }

  /*
  for (int i=0; i<1+988*24; i++) {
    cout << "superVertex[" << i << "] is " << superVertices[i] << endl;
  }
  for (int i=0; i<36*988+1; i++) {
    cout << "superIndex[" << i << "] is " << superIndices[i]; // what's stored in superIndices[i] only uses 8 spots but jumps by 36 every new cube
    cout << " which points to " << superVertices[superIndices[i]*3+0] << ", "  << superVertices[superIndices[i]*3+1] << ", "  << superVertices[superIndices[i]*3+2] << endl;
  }*/

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

  // Then do the same for all audio
  // NOTE: Commenting out audio has no effect on lag.
  initSfx(argc, argv);
  initMusic(argc, argv);
  
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
  // Then set rotation stuff
  playerDirection[n] = (360*(getPlayer(n)->getDirection())/(2*M_PI));
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

float getFPSRate() {
  float result = FPS*1.0/lastFPS;
  return result*0.5+1.0*0.5;
}