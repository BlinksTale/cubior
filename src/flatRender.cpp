/*
 * flatRender.cpp
 * by Brian Handy
 * 1/20/12
 * 2d Visuals for cubior
 */
//#include "GLee.h" // must be included before any inclusion of gl.h
#include "GLee.h"
#include "flatRender.h"
#include "music.h"
#include "gameplay.h"
#include "keyboard.h"
#include "cubeShape.h"
#include "goalShape.h"
#include "cubiorShape.h"
#include "image.h"
#include "creditsReader.h"
#include "music.h" // to show visuals in menus for music volume
#include "sfx.h" // to show visuals in menus for sfx volume
#include "networking.h" // to show visuals in menus for ip address of networking

#define _USE_MATH_DEFINES
#include <math.h> // for M_PI

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath> // for rand
#include <string> // for credits
#include <cstring>
#include <algorithm>
#include <stdio.h> // for pauseText
#include <stdlib.h> // for itoa
#include <time.h> // for printing timestamps
//#include <sys/time.h> // for linux time
#include <fstream> // for loading shader files

#include "ResourcePath.hpp" // to load (in XCode for Mac) from app's resource folder using the SFML file (combined with ResourcePath.mm)

// Starting values that change often in testing
bool fullscreen = true;
bool printFPS = false;
bool drawTriangles = true; // as opposed to just draw lines and vertices
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

// Shader uniforms (again, now both using Windows value since newer Macs have same issue)
//#ifdef __APPLE_CC__
//float shaderTinyAmount = 0.000001; // five zeros on a mac
//#else
float shaderTinyAmount = 0.0000001; // must have six zeros and a one on windows
// must be less than the cubeShape's verticies radius
// #endif
float shaderTime = 0.0;

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
// KEEP: textImage logoImage keyboardControlsImage gamepadControlsImage

// Images for HUD, Menu, etc
Image textImage, logoImage, keyboardControlsImage, gamepadControlsImage;
float menuSize = 0.75;
int menuSpacing = 200;

// Credits vars! Make sure the credits always start at the bottom for every player
int creditsTimer[4]; // when credits should start
bool creditsRecently[4]; // if we were just on credits or not
std::string* loadedCredits;

// Code from tutorial by thecplusplusguy at
// https://www.youtube.com/watch?v=pE9ZDNcg3kw
void loadFile(const char* fn, string& str) {
    std::ifstream in(fn);
    if (!in.is_open()) {
        cout << "The file " << fn << " cannot be opened" << endl;
        return;
    }
    char tmp[300];
    while (!in.eof()) {
        in.getline(tmp, 300);
        str += tmp;
        str += '\n';
    }
}
unsigned int loadShader(string& source, unsigned int mode) {

    unsigned int id;
    id=glCreateShader(mode);
    const char* csource = source.c_str();
    const int intLen = sizeof(source);
    const GLint* length = &intLen;

    glShaderSource(id, 1, &csource, NULL);
    glCompileShader(id);
    char error[1000];
    glGetShaderInfoLog(id, 1000, NULL, error);
    if (error[0] != '\0') {
        cout << "Compile status: " << error << endl;
    }
    return id;
}

const int shaderCount = 10;
unsigned int vs[shaderCount], fs[shaderCount], program[shaderCount];

void initShader(const char* vname, const char* fname, int i) {
    string source;
    loadFile(vname, source);
    vs[i] = loadShader(source, GL_VERTEX_SHADER);
    string source2;
    loadFile(fname, source2);
    fs[i] = loadShader(source2, GL_FRAGMENT_SHADER);
    
    program[i] = glCreateProgram();
    glAttachShader(program[i], vs[i]);
    glAttachShader(program[i], fs[i]);

    glLinkProgram(program[i]);
    glUseProgram(program[i]);
}

void clean()
{
    for (int i=0; i<shaderCount; i++) {
        glDetachShader(program[i], vs[i]);
        glDetachShader(program[i], fs[i]);
        glDeleteShader(vs[i]);
        glDeleteShader(fs[i]);
        glDeleteProgram(program[i]);
    }
}

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
  if (printFPS && !fullscreen) { cout << "FPS: \t" << getFPS() << endl; }
  else if (printFPS) { getFPS(); }
  //cout << "Since last frame ended: " << getTimePassed() << endl;
  
  glScissor(0,0,windowWidth,windowHeight);
  glViewport(0,0,windowWidth,windowHeight);

  // Make sure background is black first, then draw on top of it
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // do not understand order, but this works
  
  // Only draw Cubior views if game running
  if (getStarted()) {
    
    int cubiorsPlaying = getCubiorsPlaying();
    int cubiorsOnline = getCubiorsOnline();
    int cubiorsLocal = cubiorsPlaying - cubiorsOnline;
    
    //cout << "Before the loop calls:  " << getTimePassed() << endl;
    // Draw all playing Cubior views
    for (int i=0; i<getCubiorCount(); i++) {
    
      if (getCubiorPlaying(i) && !getCubiorOnline(i)) {
        //cout << "To setup player " << i << ": \t" << getTimePassed() << endl;
      
        // Figure out splitscreen values
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
        //if (!getCubiorPlaying(altX)
        //  &&!getCubiorPlaying(altY)
        //  &&!getCubiorPlaying(altXY)) {

        // Will splitscreen make your view twice as wide?
        bool doubleWidth = false; // assume that it will not!

        // Splitscreen math below!
        if (cubiorsLocal == 1) {
          // All alone? Fill all the screen space!
          viewW *= 2;
          viewH *= 2;
          posX = 0;
          posY = 0;
          setPerspective(2,2);
        } else if (cubiorsLocal == 4) {
          // Quick exit for 4player mode, run faster!
          setPerspective(1,1);
        } else if (cubiorsLocal == 2) {
          // Both flush left
          posX = 0;
          // Both are full width, no height
          viewW *= 2;
          doubleWidth = true;
          // Figure out who the other is
          int other;
          for (int j=0; j<4; j++) {
            if (j != i && getCubiorPlaying(j) && !getCubiorOnline(j)) {
              other = j;
            }
          }
          // Second of the two? Put on bottom, else top
          posY = other > i ? windowHeight*1/2+1 : -1;
          setPerspective(2,1);
        } else if (!getCubiorLocal(altY) && !getCubiorLocal(altXY)) {
          // Space Vertically? Fill the height
          viewH *= 2;
          posY = 0;
          setPerspective(1,2);
        } else if (!getCubiorLocal(altX)) {
          // Space Horizontally? Fill the width
          viewW *= 2;
          doubleWidth = true;
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

        //if (!getGameplayRunning()) {
          // Lastly, draw menu system for player (if paused, long term)
      
        if (!getGameplayRunning()) {
            glEnable(GL_TEXTURE_2D);
            drawMenu(i,doubleWidth);
            glDisable(GL_TEXTURE_2D);
        }
      
        //}
        //cout << "To display for " << i << ": \t" << getTimePassed() << endl;
      
      }
    }
  } else {
      // Nobody playing yet?
      
      glScissor(0,0,windowWidth,windowHeight);
      glViewport(-windowWidth/2,0,windowWidth*2,windowHeight);
      displayMenu();
      if (!getGameplayRunning()) {
        glEnable(GL_TEXTURE_2D);
        drawMenu(0,windowWidth);
        glDisable(GL_TEXTURE_2D);
      }
  }
  //cout << "Player loop ended: \t" << getTimePassed() << endl;

  // End with a quick flush, to draw faster
  // EDIT: No need for flush! This is only for when graphics card is on other side of the networks
  //glFlush();

  // just before swap buffers, print on top the textures!
  //drawMenu(0);

  //cout << "Flush: \t\t\t" << getTimePassed() << endl;
  glutSwapBuffers(); // because using double buffer, must swap buffers
  //cout << "Swap: \t\t\t" << getTimePassed() << endl;
  //cout << "End: \t\t\t" << getTimePassed() << endl;

}

// Draw the start screen
void displayMenu() {
    //cout << "Display for " << player << ":    \t\t" << getTimePassed() << endl;
    
    // Paint background cyan to neon blue
    glClearColor(getMapRed(), getMapGreen(), getMapBlue(), 0.0f); // do this here to allow black borders
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // must set this here to undo silhouette's effects later
    glLoadIdentity();
    
    glPushMatrix();
    // Zoom camera out,
    glScalef(0.001,0.001,0.001);
    // Set camera angle and pos, based off averages
    glRotatef(0,1.0,0.0,0.0);
    glRotatef(0,0.0,1.0,0.0);
    glRotatef(0,0.0,0.0,1.0);
    // then pull back and up to see cubes
    glTranslatef(0,0,0);
    
    int n; // errors if not declared before if statement
    if (!getGameplayRunning() && getLastPause() == -1) {
        // Vars for all menu bitmap text
        int pW = 0;//1000;
        int pH =-1000;
        int pD = 0; // was -1000 when angleX was 0, now angleX is 270
        
        // Where the credits are for scrolling, we need a time basis
        int time;
        time = clock();
        
        // Runs too fast on Apple, maybe clock returns different results
        // so we have to slow it down here
#ifdef __APPLE_CC__
        time /= 160.0;
#endif
        
        if (!getStarted()) {
            // START SCREEN temp fix for IGF demo
            //n=sprintf(pausedText, "Cubior");
            //printString(pausedText,cameraPointer[player]->getMeanX()+pW+75*widthVsExpected,cameraPointer[player]->getMeanY()+pH,cameraPointer[player]->getMeanZ()+pD+100*heightVsExpected);
            int startingHeight = 70;
            n=sprintf(pausedText, "by Brian Handy");
            printStringFlat(pausedText,165,-25 - startingHeight);
            n=sprintf(pausedText, "Sound by Rolando Nadal");
            printStringFlat(pausedText,260,-0 - startingHeight);
            n=sprintf(pausedText, "Music by Waterflame");
            printStringFlat(pausedText,225,25 - startingHeight);
            n=sprintf(pausedText, "Copyright 2013");
            printStringFlat(pausedText,225,450 - startingHeight);
            
            //n=sprintf(pausedText, "Press Start/Enter!");
            //printString(pausedText,cameraPointer[player]->getMeanX()+pW+215*widthVsExpected,cameraPointer[player]->getMeanY()+pH,cameraPointer[player]->getMeanZ()+pD-100*heightVsExpected);
            //n=sprintf(pausedText, "(up to four players can join using gamepads)");
            //printString(pausedText,cameraPointer[player]->getMeanX()+515*widthVsExpected,cameraPointer[player]->getMeanY(),cameraPointer[player]->getMeanZ()+pD-125*heightVsExpected);
            
        /*} else if (getMenu(player) == 5) { // Credits menu
            // If first entering credits, reset timer
            if (!creditsRecently[player]) {
                creditsTimer[player] = time;
                creditsRecently[player] = true;
            }
            // Then pos credits based on time in relation to when we entered
            int creditsHeight = -500 + ((time - creditsTimer[player]) % 91000)/25;
            // And draw credits!
            for (int i=0; i<200; i++) {
                n=sprintf(pausedText, loadedCredits[i].c_str());
                printStringFlat(pausedText,player,165,-50+25*i - creditsHeight); // remove 165?
            }
        */
        }
        
        /*if (creditsRecently[player] && getMenu(player) != 5) {
            // Make sure we don't think the player has been looking at the credits
            // if they are somewhere else
            creditsRecently[player] = false;
        }*/
    }
    
    // Then on top, draw pause text
    if (!getGameplayRunning() && getLastPause() == -1) {
        n=sprintf(pausedText, "by Brian Handy");
        printString(pausedText,0,200,0);
        n=sprintf(pausedText, "Sound by Rolando Nadal");
        printString(pausedText,0,100,0);
        n=sprintf(pausedText, "Music by Waterflame");
        printString(pausedText,0,0,0);
        //n=sprintf(pausedText, "Press Start/Enter!");
        //printString(pausedText,cameraPointer[player]->getMeanX(),cameraPointer[player]->getMeanY()-1400,cameraPointer[player]->getMeanZ());
        
    }
    
    glPopMatrix();
    
}

// Draw an entire player's screen
void displayFor(int player) {
  //cout << "Display for " << player << ":    \t\t" << getTimePassed() << endl;
  
  // Paint background cyan to neon blue
  glClearColor(getMapRed(), getMapGreen(), getMapBlue(), 0.0f); // do this here to allow black borders
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW); // must set this here to undo silhouette's effects later
  glLoadIdentity();
  
  glPushMatrix();
  // Zoom camera out,
  glScalef(0.001,0.001,0.001);
  // Set camera angle and pos, based off averages
  glRotatef(-1.0*cameraPointer[player]->getMeanAngleX(),1.0,0.0,0.0);
  glRotatef(-1.0*cameraPointer[player]->getMeanAngleY(),0.0,1.0,0.0);
  glRotatef(-1.0*cameraPointer[player]->getMeanAngleZ(),0.0,0.0,1.0);
  // then pull back and up to see cubes
  glTranslatef(-1.0*cameraPointer[player]->getMeanX(),-1.0*cameraPointer[player]->getMeanY(),-1.0*cameraPointer[player]->getMeanZ());
    
  // About to draw scenery? Draw using special scenery style/shader
  glUseProgram(program[1]);
    
    // Update time
    GLint time = glGetUniformLocation(program[2], "time");
    shaderTime += 0.05;
    glUniform1f(time, shaderTime);
    // Init shader uniform values too
    glUniform1f(glGetUniformLocation(program[1], "tinyAmount"), shaderTinyAmount);

  //cout << "Starting half " << player << ":  \t\t" << getTimePassed() << endl;
  drawAllCubes(player);
  
  // Done drawing scenery? Start drawing default style again
  glUseProgram(program[0]);
    
  //cout << "Last half " << player << ":     \t\t" << getTimePassed() << endl;
  // Draw goal second to last since we want it in front of player for silhouette
  if (drawOutlines) { drawGoalOutline(); }
  drawGoal();

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
  
    
  // Then draw all shadows in order of height!
  drawAllShadows(player);

  // Switch to "no shader" for menus
  glUseProgram(0);
  int n; // errors if not declared before if statement
  if (!getGameplayRunning() && getLastPause() == -1) {
    // Vars for all menu bitmap text
    int pW = 0;//1000;
    int pH =-1000;
    int pD = 0; // was -1000 when angleX was 0, now angleX is 270
    
    // Where the credits are for scrolling, we need a time basis
    int time;
    time = clock();

    // Runs too fast on Apple, maybe clock returns different results
    // so we have to slow it down here
    #ifdef __APPLE_CC__
    time /= 160.0;
    #endif

    if (getMenu(player) == 0) {
      // START SCREEN temp fix for IGF demo
      //n=sprintf(pausedText, "Cubior");
      //printString(pausedText,cameraPointer[player]->getMeanX()+pW+75*widthVsExpected,cameraPointer[player]->getMeanY()+pH,cameraPointer[player]->getMeanZ()+pD+100*heightVsExpected);
      int startingHeight = 70;
      n=sprintf(pausedText, "by Brian Handy");
      printStringFlat(pausedText,player,165,-25 - startingHeight);
      n=sprintf(pausedText, "Sound by Rolando Nadal");
      printStringFlat(pausedText,player,260,-0 - startingHeight);
      n=sprintf(pausedText, "Music by Waterflame");
      printStringFlat(pausedText,player,225,25 - startingHeight);
      n=sprintf(pausedText, "Copyright 2013");
      printStringFlat(pausedText,player,225,450 - startingHeight);

      //n=sprintf(pausedText, "Press Start/Enter!");
      //printString(pausedText,cameraPointer[player]->getMeanX()+pW+215*widthVsExpected,cameraPointer[player]->getMeanY()+pH,cameraPointer[player]->getMeanZ()+pD-100*heightVsExpected);
      //n=sprintf(pausedText, "(up to four players can join using gamepads)");
      //printString(pausedText,cameraPointer[player]->getMeanX()+515*widthVsExpected,cameraPointer[player]->getMeanY(),cameraPointer[player]->getMeanZ()+pD-125*heightVsExpected);
  
    } else if (getMenu(player) == 5) { // Credits menu
      // If first entering credits, reset timer
      if (!creditsRecently[player]) {
        creditsTimer[player] = time;
        creditsRecently[player] = true;
      }
      // Then pos credits based on time in relation to when we entered
      int creditsHeight = -500 + ((time - creditsTimer[player]) % 91000)/25;
      // And draw credits!
      for (int i=0; i<200; i++) {
        n=sprintf(pausedText, loadedCredits[i].c_str());
        printStringFlat(pausedText,player,165,-50+25*i - creditsHeight); // remove 165?
      }
      /*n=sprintf(pausedText, "Programming:");
      printStringFlat(pausedText,player,165,-50 - creditsHeight);
      n=sprintf(pausedText, "Brian Handy");
      printStringFlat(pausedText,player,260,-25 - creditsHeight);*/
      
    }

    if (creditsRecently[player] && getMenu(player) != 5) {
      // Make sure we don't think the player has been looking at the credits
      // if they are somewhere else
      creditsRecently[player] = false;
    }
  }

    // Shadows used to be here... moved now to up above.
    
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
    }
    //printString(pausedText,playerX[player],playerY[player]+200,playerZ[player]);
  }
  // And player stats (wip/temp)
  // Commented out so that these values can easily be displayed again if necessary 
  // if (getPlayer(0)->getGrounded()) { printString("grounded",0,140,0); } else { printString("flying",0,120,0); }

  
  // Then on top, draw pause text
  if (!getGameplayRunning() && getLastPause() == -1) {
      n=sprintf(pausedText, "by Brian Handy");
      printString(pausedText,cameraPointer[player]->getMeanX(),cameraPointer[player]->getMeanY()+200,cameraPointer[player]->getMeanZ());
      n=sprintf(pausedText, "Sound by Rolando Nadal");
      printString(pausedText,cameraPointer[player]->getMeanX(),cameraPointer[player]->getMeanY()+100,cameraPointer[player]->getMeanZ());
      n=sprintf(pausedText, "Music by Waterflame");
      printString(pausedText,cameraPointer[player]->getMeanX(),cameraPointer[player]->getMeanY()+000,cameraPointer[player]->getMeanZ());
      //n=sprintf(pausedText, "Press Start/Enter!");
      //printString(pausedText,cameraPointer[player]->getMeanX(),cameraPointer[player]->getMeanY()-1400,cameraPointer[player]->getMeanZ());

    }
  
  glPopMatrix();
  


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
  if (getCubiorPlaying(n)) {
    // Then animate player rotation
	float changeMin = 5.0;
	// Only an issue for changeZ = -20 to changeZ = 0. Not sure why.
    if (meanChangeX[n] > changeMin || meanChangeX[n] < -changeMin || meanChangeZ[n] > changeMin || meanChangeZ[n] < -changeMin) {
      playerAngleDivisor[n] = ((meanChangeZ[n] != 0.0) ? meanChangeZ[n] : 0.01);
      playerAngleNumerator[n] = meanChangeX[n];
    }
    lastChangeZ[n] = meanChangeZ[n];
    // FINALLY, update player angle, but only once!
    // Find new angle
    float oldAngle = playerRotationMean(n);
    float newAngle = oldAngle;
    
    // NEW WAY BELOW
    if (newAngle != playerDirection[n]) {
      newAngle = playerDirection[n];
      // Then modify it to match current mean
      while (newAngle > oldAngle + 180) { newAngle -= 2*180; }
      while (newAngle < oldAngle - 180) { newAngle += 2*180; }
    }
    playerRotationAngle[n][currentPlayerRotation[n]] = newAngle;

    // Actually, overriding past stuff here (new era!)
    // With new way, we instead assign the angle of joystick/current str dir
    //playerRotationAngle[n][currentPlayerRotation[n]] = newAngle;
    // Not in use actually, since instead using playerDirection to communicate this

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
    float stretchSize = 0.0;//1.5;//5.0;
    
    float avg = playerRotationMean(n);
    float avgRad = avg*M_PI*2/360.0;
    //cout << avgRad << " and cos is " << cos(avgRad) << endl;
    
    float valueX = cos(avgRad)*changeX[n] - sin(avgRad)*changeZ[n];
    float valueZ = sin(avgRad)*changeX[n] + cos(avgRad)*changeZ[n];
    
    float deltaX = stretchSize*abs(valueX);
    float deltaY = stretchSize*abs(changeY[n]);
    float deltaZ = stretchSize*abs(valueZ);

    //cout << "newAngle is " << newAngle << endl;
  //cout << "avg is " << avg << endl;
  glRotatef(avg,0.0,1.0,0.0);
    // X/Z stretching included
    glScalef(deltaX-(deltaY+deltaZ)/2+100,deltaY-(deltaX+deltaZ)/2+100,deltaZ-(deltaX+deltaY)/2+100);

    // X/Z stretching NOT included
    //glScalef(-(deltaY)/2+100,deltaY+100,-(deltaY)/2+100);
    
    //glScalef(10.0,100.0,100.0);
    
  // And make player bigger
  //glScalef(100.0,100.0,100.0);
    // playerScaleX
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
  if (getCubiorPlaying(n)) {
    playerPreDraw(n);
    cubiorShape[n].drawCubior(n);
    playerPostDraw(n);
  }
}

void drawPlayerSilhouette(int n) {
  if (getCubiorPlaying(n)) {
    playerPreDraw(n);
    cubiorShape[n].drawCubiorSilhouette();
    playerPostDraw(n);
  }
}

// Same as drawPlayer, but shadows separately
void drawPlayerShadow(int n) {
  if (getCubiorPlaying(n)) {
    playerPreDraw(n);
    cubiorShape[n].drawShadow();
    playerPostDraw(n);
  }
}


// Same as drawPlayer, but shadows separately
void drawPlayerOutline(int n) {
  if (getCubiorPlaying(n)) {
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
    if (drawTriangles) {
      glDrawElements(GL_TRIANGLES, 6*facesVisible, GL_UNSIGNED_INT, superIndices);
    } else {
        glDrawElements(GL_LINES, 6*facesVisible, GL_UNSIGNED_INT, superIndices);
    }
    
    glColorPointer(3, GL_FLOAT, 0, topColors);
    glVertexPointer(3, GL_FLOAT, 0, topVertices);
    if (drawTriangles) {
        glDrawElements(GL_TRIANGLES, 6*topFacesVisible, GL_UNSIGNED_INT, topIndices);
    } else {
        glDrawElements(GL_LINES, 6*topFacesVisible, GL_UNSIGNED_INT, topIndices);
    }
    
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
// (tick or void draw or void draw() or tick() etc)
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

  // Check for errors
  // Code copied from datenwolf on StackOverflow:
  // http://stackoverflow.com/questions/7857527/why-is-gldrawelements-giving-me-gl-out-of-memory
  /*int er = 0;
  while( (er = glGetError()) != GL_NO_ERROR ) {
    std::cerr << gluErrorString(er) << std::endl;
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
    cubiorShape[i].setSelf(getPlayer(i));
    updatePlayerGraphic(i);
  }

  // Get the Cube count
  cubeNum = getCubeCount();
  
  //Reset!
  cubesVisible = 0;
  facesVisible = 0;
  topFacesVisible = 0;
  shadowsVisible = 0;

  // Initialize Scenery Cube Visual Vals
  for (int i=0; i<cubeNum; i++) {
    // Only draw visible cubes
    if (!getCube(i)->isInvisible()) {
      cubeCollision[i] = false;
      updateCubeGraphic(i);
      // Find the color! Based both on location (checker pattern) and map info (assigned color)
      //int altSize = 400; // how wide the checker patterns are
      //bool alternatingSpot =( // calculate if given a dark checker spot or not
      //    (cubeX[i]<0)^((int(abs(cubeX[i]+1))%(altSize*2)<altSize))
      //  ) ^ (
      //    (cubeY[i]<0)^((int(abs(cubeY[i]+1))%(altSize*2)<altSize))
      //  )^ (
      //    (cubeZ[i]<0)^((int(abs(cubeZ[i]+1))%(altSize*2)<altSize))
      //  );
      bool alternatingSpot = getCube(i)->getAlternatingSpot();
      cubeShape[i].setMaterial(getCube(i)->getMaterial());
      // Yellow color assigned
      if (getCube(i)->getMaterial()==9) {
        cubeShape[i].initVisuals(0.95,1.00,0.50, 0.9,1.0,0.5, 0*0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Polar color assigned
      if (getCube(i)->getMaterial()==8) {
        cubeShape[i].initVisuals(1.0,1.00,1.0, 0.0,0.0,0.0, 0*1.0,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      }
      // Cave color assigned
      } else if (getCube(i)->getMaterial()==7) {
        cubeShape[i].initVisuals(0.10,0.15,0.65, 0.20,0.10,0.55,0*0.25,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Bridge color assigned
      } else if (getCube(i)->getMaterial()==6) {
        cubeShape[i].initVisuals(0.80,0.52,0.25, 0.90,0.62,0.35, 0*0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Canyon color assigned
      } else if (getCube(i)->getMaterial()==5) {
        cubeShape[i].initVisuals(0.90,0.35,0.11, 1.0,0.58,0.41, 0*0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Rock color assigned
      } else if (getCube(i)->getMaterial()==4) {
        cubeShape[i].initVisuals(0.4,0.4,0.4, 0.5,0.5,0.5, 0*0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Snow color assigned
      } else if (getCube(i)->getMaterial()==3) {
        cubeShape[i].initVisuals(0.87,0.87,1.00, 1.0,1.0,1.0, 0*0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Castle Wall color assigned
      } else if (getCube(i)->getMaterial()==2) {
        cubeShape[i].initVisuals(0.52,0.62,0.54, 0.9,0.5,0.5, 0*0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      // Grass color assigned as default
      } else {
        cubeShape[i].initVisuals(0.92,0.62,0.04, 0.0,0.9,0.0, 0*0.5,alternatingSpot,cubeY[i]<=0  && abs(cubeZ[i])!=playableWidth/2);
      }
      // Grab neighbors, position, and shadow for cube visual/shape object
      cubeShape[i].setSelf(getCube(i));
      cubeShape[i].setNeighbors(getCube(i)->getVisibleNeighbors());
      cubeShape[i].setNeighborObjects(getCube(i)->getVisibleNeighborObjects());
      cubeShape[i].permanentPosition(cubeX[i], cubeY[i], cubeZ[i]);
      cubeShape[i].setShadow(getShadow(i));

      
      // Finally, if it has a shadow, remember that too
      if (cubeShape[i].hasShadow()) {
        // OK, add the cube's vertices
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
    }
  }

  // End of part 1 of cube initialization. Everything has a cube shape now, so next, make them more efficient
  // and add them to the super arrays

  
  // Part 2: Finish Initializing Scenery Cube Visual Vals
  for (int i=0; i<cubeNum; i++) {
    // Only draw visible cubes, must also be non-duplicates
    if (!getCube(i)->isInvisible() && !getCube(i)->getDuplicateNeighbor()) {
      
      // TODO: Now that neighbors are set, we should change all the vertices for cubes with like-neighbors
      cubeShape[i].removeDuplicateNeighbors();

      // If even one face is visible, include the cube's vertices
      if (cubeShape[i].hasVisibleFace()) {
        // OK, add the cube's vertices and colors
        for (int vertex=0; vertex<24; vertex++) {
          superVertices[cubesVisible*24+vertex] = cubeShape[i].getVertex(vertex); // add vertices
          superColors[cubesVisible*24+vertex] = cubeShape[i].getColor(vertex);    // add colors
        }
        // Alright, now find that face. If it has no neighbors, add it to indicies! (which vertices to draw)
        for (int face=0; face<6; face++) {
          if (cubeShape[i].hasFace(face) && face != 2) { // don't draw top face here
            // Add all 6 indices for whichever face we have decided to draw
            for (int vertex=0; vertex<6; vertex++) {
              // in the index for the current face, plus the number of that vertex,
              // set the index to be from that face and vertex for that cube.
              // the 8 at the end is since we are using all 8 vertices (cube's corners) every time
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

// Load data into Image objects
void initMenu() {
  // Extra path ensures Resources folder is accessed for .app on Mac
  std::string extraPath = "";
  #ifdef __APPLE_CC__
    extraPath = resourcePath();
  #endif  
    
  // These need to be converted to const char* with c_str for Image to accept them
  textImage            = Image((extraPath + "./images/cubiorTextSpriteSheet.png").c_str(),1);
  logoImage            = Image((extraPath + "./images/CubiorLogo720.png").c_str(),1);
  keyboardControlsImage  = Image((extraPath + "./images/CubiorControlsKeyboard720.png").c_str(),1);
  gamepadControlsImage   = Image((extraPath + "./images/CubiorControlsGamepad720.png").c_str(),1);

}

// Draws the 2D PNGs themselves as textures
void drawMenu(int i, bool doubleWidth) {
    int midY = 0;
  // Don't use whole viewport since different menu for every player
  //glViewport(0, 0, windowWidth, windowHeight); // don't need?
  glMatrixMode(GL_PROJECTION); // necessary
  glLoadIdentity(); // this one seems to be necessary
  GLfloat aspect = ((GLfloat)windowWidth / (GLfloat)windowHeight)/(1600.0/1050.0) * (1 + doubleWidth);
  //gluPerspective(45.0, aspect*windowWidth/windowHeight, 0.050, 100.0);
  if (aspect > 1.0) {
    glOrtho(0, 1600*aspect, 1050, 0, -1, 1); // necessary
  } else {
    glOrtho(0, 1600, 1050/aspect, 0, -1, 1); // necessary
      midY = 0.5*1050/aspect-1050/2;
  }
  //glOrtho(0, 1600*aspect, 1050, 0, -1, 1); // necessary
  glMatrixMode(GL_MODELVIEW); // don't need?
  glLoadIdentity(); // don't need?
  
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_ALPHA_TEST); // no visible effect yet
  
  // Enable the texture for OpenGL.
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // slight smoothing when smaller than norm
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // no smoothing when full size and up
  
  // Still unsure of functionality, but known importance!
  // glColor4ub brings back color to the PNG where it was black before
  glColor4ub(255, 255, 255, 255);

  // Where the logo is for bouncing
  int time;
  time = clock();

  // Runs too fast on Apple, maybe clock returns different results
  // so we have to slow it down here
  #ifdef __APPLE_CC__
  time /= 160.0;
  #endif
  
  // Only draw any of it if paused
    // Something will always be moving/rotating
    int option = getOption(i);
    float rotation = 5.0*sin(time/300.0);
    textImage.prepDraw(); // must do this before using writeWord
    // Start screen?
    if (/*getLastPause() == -1 && */getMenu(i) == 0) {
        // Splash screen
        
        //writeWord("JohnAbdoujOHNaBDOU", -650, -300, aspect);
        // J, A, U, and j are being weird
        //writeWord("jjajjaajajaaja", -350, -100, aspect);
        //writeWord("thequickbrownfoxju", -650, 100, aspect); //
        //writeWord("mpedoverthelazydog", -700, 300, aspect);
        //writeWord("THEQUICKBROWNFOXJU", -650, 100, aspect); //
        //writeWord("MPEDOVERTHELAZYDOG", -700, 300, aspect);
        //string myString = "The number is ";
        //char number[21];
        //sprintf(number, "%d", time);
        //writeWord(myString + number, 0, 100, aspect, false); //
        //writeWord("http://192.368.0.457/", 0, 100, aspect); //
        //writeWord("How are you?", -650, 100, aspect); //
        
        
        // Blink the press start/enter image
        if (time%1200 < 900) {
          if (joystickConnected() && time > 8000) {
            writeWord("Press Start",0,midY*2+330,aspect,false); // was 380
          } else {
            writeWord("Press Enter",0,midY*2+330,aspect,false);
          }
        }
    } else if (getMenu(i) == 1) { // Start Menu
        //startImage.draw( 0,-1.5*menuSpacing,aspect,(option==0)*rotation);
        writeWord("Start",  0,midY-1.5*menuSpacing,aspect,(option==0)*rotation);
        writeWord("Options",0,midY-0.5*menuSpacing,aspect,(option==1)*rotation);
        writeWord("Credits",0,midY+0.5*menuSpacing,aspect,(option==2)*rotation);
        writeWord("Quit",   0,midY+1.5*menuSpacing,aspect,(option==3)*rotation);
    // Regular pause!
    } else if (getMenu(i) == 2) { // Main Pause Menu
      writeWord("Resume",  0,midY-1.5*menuSpacing,aspect,(option==0)*rotation);
      writeWord("Drop Out",0,midY-0.5*menuSpacing,aspect,(option==1)*rotation);
      writeWord("Options", 0,midY+0.5*menuSpacing,aspect,(option==2)*rotation);
      writeWord("Quit",    0,midY+1.5*menuSpacing,aspect,(option==3)*rotation);
    } else if (getMenu(i) == 3) { // Pause Options Menu
      writeWord("Camera Controls", 0,midY-2.0*menuSpacing,aspect,false);
      if (getIndependentMovement(i)) {
        writeWord(":Pro:", 0,midY-1.0*menuSpacing,aspect,(option==0)*rotation);
      } else {
        writeWord(":Easy:",0,midY-1.0*menuSpacing,aspect,(option==0)*rotation);
      }
      writeWord("Controls",0,midY+0.0*menuSpacing,aspect,(option==1)*rotation);
      writeWord("Volume",  0,midY+1.0*menuSpacing,aspect,(option==2)*rotation);
      writeWord("Back",    0,midY+2.0*menuSpacing,aspect,(option==3)*rotation);
    } else if (getMenu(i) == 4) { // Controls Display
      if (getJustHitStart()) {
        writeWord("Start", 0,midY*2+400,aspect,(option==0)*rotation);
      } else {
        writeWord("Back",  0,midY*2+400,aspect,(option==0)*rotation);
      }
    } else if (getMenu(i) == 5) { // Start Controls Display
      //creditsTextImage.draw(0,-100,aspect,false);
      // ATTEMPT AT REPLACING CREDITS IMAGE W/ REAL TEXT
      // except that this gets drawn in the same place the splash screen text does
      // (Ctrl+F "Rolando" will find that part of the code)
      writeWord("Back",   0, midY*2+400,aspect,(option==0)*rotation);
    } else if (getMenu(i) == 6) { // Volume Controls Display
      writeWord("Music",  0,midY-2.0*menuSpacing,aspect,false);
      switch (getMusicVolumeNum()) {
      case 0:
        writeWord("Off", 0,midY-1.0*menuSpacing,aspect,(option==0)*rotation); break;
      case 1:
        writeWord("Low", 0,midY-1.0*menuSpacing,aspect,(option==0)*rotation); break;
      case 3:
        writeWord("High",0,midY-1.0*menuSpacing,aspect,(option==0)*rotation); break;
      default:
        writeWord("Med", 0,midY-1.0*menuSpacing,aspect,(option==0)*rotation); break;
      }
      writeWord("Sound", 0,midY+0.0*menuSpacing,aspect,false);
      switch (getSoundVolumeNum()) {
      case 0:
        writeWord("Off", 0,midY+1.0*menuSpacing,aspect,(option==1)*rotation); break;
      case 1:
        writeWord("Low", 0,midY+1.0*menuSpacing,aspect,(option==1)*rotation); break;
      case 3:
        writeWord("High",0,midY+1.0*menuSpacing,aspect,(option==1)*rotation); break;
      default:
        writeWord("Med", 0,midY+1.0*menuSpacing,aspect,(option==1)*rotation); break;
      }
      writeWord("Back",  0,midY+2.0*menuSpacing,aspect,(option==2)*rotation);
    } else if (getMenu(i) == 7) { // Local or Network Menu
        writeWord("Local",       0,midY-1.5*menuSpacing,aspect,(option==0)*rotation);
        writeWord("LAN",         0,midY-0.5*menuSpacing,aspect,(option==1)*rotation);
        writeWord("Online",      0,midY+0.5*menuSpacing,aspect,(option==2)*rotation);
        writeWord("Back",        0,midY+1.5*menuSpacing,aspect,(option==3)*rotation);
    } else if (getMenu(i) == 8) { // Host or Search Menu
        writeWord("Host",        0,midY-1.0*menuSpacing,aspect,(option==0)*rotation);
        writeWord("Join",        0,midY+0.0*menuSpacing,aspect,(option==1)*rotation);
        writeWord("Back",        0,midY+1.0*menuSpacing,aspect,(option==2)*rotation);
    } else if (getMenu(i) == 9) { // IP Address Menu / Connect Online Menu
        // Data for Ip Address
        int focusStart = 0; // getFocus(0) has to figure out its size and all sizes before it
        int focusEnd = -1;
        char address [50];
        char addressArray[4][4];
        
        for (int j=0; j<4; j++) {
            //sprintf(addressArray[j], "%d", getIpAddress(j));
            if (j == 0) {
                sprintf(address, "%s", addressArray[j]);
            } else {
                sprintf(address, "%s.%s", address, addressArray[j]);
            }
            if (getFocus(0) > j) {
                focusStart += strlen(addressArray[j]);
                focusStart++; // to skip periods
            }
            if (getFocus(0) >= j) {
                focusEnd += strlen(addressArray[j]);
                focusEnd++; // to skip periods
            }
        }
        
        writeWord("IP Address:",       0,midY-1.5*menuSpacing,aspect,false);
        writeWordRangeRotation(address,0,midY-0.5*menuSpacing,aspect,(option==0)*rotation,focusStart,focusEnd); // can only have one rotating section at a time
        writeWord("Connect",           0,midY+0.5*menuSpacing,aspect,(option==1)*rotation);
        writeWord("Back",              0,midY+1.5*menuSpacing,aspect,(option==2)*rotation);
    }

    
    
    
    // Now the non-word images
    if (getMenu(i) == 0) {
        // Splash screen
        logoImage.draw(20.0*sin(time/1600.0),-300+10.0*sin(time/800.0),aspect,false);
    } else if (getMenu(i) == 4) { // Controls Display
        if (joystickConnected()) {
            gamepadControlsImage.draw(0,-100,aspect,false);
        } else {
            keyboardControlsImage.draw(0,-100,aspect,false);
        }
    }
  glDisable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

}

void writeWord(const string word, int x, int y, float aspect, float rotate) {
    writeWordRangeRotation(word, x, y, aspect, rotate, -1, -1);
}

void writeWordIndividualRotation(const string word, int x, int y, float aspect, float rotate, int focus) {
    writeWordRangeRotation(word, x, y, aspect, rotate, focus, focus+1);
}
                                 
void writeWordRangeRotation(const string word, int x, int y, float aspect, float rotate, int rangeStart, int rangeEnd) {
    
    int currentOffset = 0;
    int wordWidth = 0;
    int offsetX = 34;
    int offsetY = 38;
    int letter = 0;
    int line = 0;
    int slotX = 0;
    int slotY = 0;
    int characterWidth = 144;
    int characterHeight = 144;
    int spacing = 10;
    int uc = 81;
    int lc = 73;
    int vc = 67;
    int zc = 58;
    int xc = 80;
    int li = 26;
    int ui = 30;
    int tc = 45;//for t 45 and all else is 50;
    int rc = 52;//for t 45 and all else is 50;
    int uw = 130;
    int lw = 110;
    int characterWidths[70] = {
        // A, a, B, b, C, c, D, d, E, e,
          uc,lc,uc,lc,uc,lc,uc,lc,uc,lc,
        // F, f, G, g, H, h, I, i, J, j,
          uc,tc,uc,lc,uc,lc,ui,li,uc,li,
        // K, k, L, l, M, m, N, n, O, o,
          lc,lc,uc,tc,uw,lw,uc,lc,uc,lc,
        // P, p, Q, q, R, r, S, s, T, t,
          lc,lc,uc,lc,lc,rc,uc,zc,uc,tc,
        // U, u, V, v, W, w, X, x, Y, y,
          uc,lc,uc,vc,uw,lw,uc,xc,lc,lc,
        // Z, z, ., :, /,
          uc,zc,ui,ui,vc,vc,uc,uc,uc,uc,
        // 1, 2, 3, 4, 5, 6, 7, 8, 9, 0
          tc,uc,uc,uc,uc,uc,uc,uc,uc,uc,
    };
    
    //int slots[word.length()];
    int *slotsP = new int[word.length()];
    
    for (int i=0; i<word.length(); i++) {
        int asciiValue = (int)(word[i]);
        int slot = -1;
        //cout << "Letter value for " << word[i] << " is " << asciiValue << endl;
        if (asciiValue >= 65 && asciiValue <= 90) { // uppercase
            slot = (asciiValue - 65)*2;
        } else if (asciiValue >= 97 && asciiValue <= 122) { // lowercase
            slot = (asciiValue - 97)*2+1;
        } else if (asciiValue >= 48 && asciiValue <= 57) { // numbers
            slot = (asciiValue - 39)%10+60;
        } else if (asciiValue == 46) { // .
            slot = 52;
        } else if (asciiValue == 58) { // :
            slot = 53;
        } else if (asciiValue == 47) { // /
            slot = 54;
        } else if (asciiValue == 32) { // space
            slot = 55;
        }
        slotsP[i] = slot;
        if (slot != -1) {
            
            slotX=slot%10;
            slotY=slot/10;
            bool isLowercaseJ = (slot == 19);
            int currentWidth = (isLowercaseJ ? offsetX : 0)+characterWidths[slotX+slotY*10];
            wordWidth += (i == word.length()-1 ? 0 : spacing)+currentWidth;//characterWidths[slotX+slotY*10];
        }
    }
    
    // Then write it
    for (int i=0; i<word.length(); i++) {
        if (slotsP[i] != -1) {
            slotX=slotsP[i]%10;
            slotY=slotsP[i]/10;
            bool isLowercaseJ = (slotsP[i] == 19);
            int currentWidth = (isLowercaseJ ? offsetX : 0)+characterWidths[slotX+slotY*10];
            textImage.draw(-wordWidth/2.0+x+currentOffset+currentWidth/2-(isLowercaseJ ? offsetX : 0),
                           y+(line)*characterHeight,
                           // Texture start:
                           (isLowercaseJ ? 0 : offsetX)+characterWidth*slotX, offsetY+characterHeight*slotY,
                           // Texture size:
                           currentWidth, characterHeight,
                           aspect, (i >= rangeStart && i < rangeEnd) || rangeStart == -1 ? rotate : 0);
            letter++;
            currentOffset += characterWidths[slotX+slotY*10]+spacing;
        }
    }

    delete[] slotsP;

}

// To setup OpenGL and GLUT in general
void initFlat(int argc, char** argv) {
  
  // Extra path ensures Resources folder is accessed for .app on Mac
  std::string extraPath = "";
  #ifdef __APPLE_CC__
    extraPath = resourcePath();
  #endif  
  
  // Read in Credits doc real quick first, before images even
  loadedCredits = CreditsReader::readCredits((extraPath + "./doc/finalCredits.txt").c_str());

  // load PNGs for menu system
  initMenu();

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
  
  // Enable shaders / init shaders
    initShader((extraPath + "./shaders/defaultShader.vert").c_str(), (extraPath + "./shaders/defaultShader.frag").c_str(), 0);
    initShader((extraPath + "./shaders/sceneryShader.vert").c_str(), (extraPath + "./shaders/sceneryShader.frag").c_str(), 1);
    initShader((extraPath + "./shaders/sceneryShader.vert").c_str(), (extraPath + "./shaders/waterfallShader.frag").c_str(), 2);
    initShader((extraPath + "./shaders/sceneryShader.vert").c_str(), (extraPath + "./shaders/outlineShader.frag").c_str(), 3);
  // Finish shaders by resetting current program
  glUseProgram(0);
    
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
    
  // And once's it's all done, delete shaders
  clean();
}

void updatePlayerGraphic(int n) {
  setPlayerGraphic(n,getPlayer(n)->getX(),getPlayer(n)->getY(),getPlayer(n)->getZ());
  // Then set rotation stuff
  //playerDirection[n] = (360*(getPlayer(n)->getDirection())/(2*M_PI));
  // Old way above, new era new way below
  // Only update graphic if recently told a new dir
  //if (getPlayer(n)->getLastToldToMove()) {
    playerDirection[n] = (360*(getPlayer(n)->getToldDirection())/(2*M_PI));
    // FIXME: new era, insert something here so that eyes squint when fighting dir
  //}
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

// Print bitmap text relative to center of screen
// Probably only works for start menus, before game begins
void printStringFlat(char *string, int player, int x, int y) {
    // Adjust positioning based on window size vs expected window size
    float heightVsExpected = -1050.0/windowHeight;
    float widthVsExpected = windowWidth/1600.0;
    
    int len, i;
    len = (int)strlen(string);
    glColor3f( 0.0f, 0.0f, 0.0f );
    glRasterPos3f(
                  cameraPointer[player]->getMeanX()+0.0f+len*3*heightVsExpected,
                  cameraPointer[player]->getMeanY()-1000.0f,
                  cameraPointer[player]->getMeanZ()+0.0f+y);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
    }
}

void printStringFlat(char *string, int x, int y) {
    // Adjust positioning based on window size vs expected window size
    float heightVsExpected = -1050.0/windowHeight;
    float widthVsExpected = windowWidth/1600.0;
    
    int len, i;
    len = (int)strlen(string);
    glColor3f( 0.0f, 0.0f, 0.0f );
    glRasterPos3f(
                  0.0f+len*3*heightVsExpected,
                  -1000.0f,
                  0.0f+y);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
    }
}

// Print bitmap text on screen
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
  // Hijacked! Used to toggle whatever we're working on now.
  //drawOutlines = !drawOutlines;
  drawTriangles = !drawTriangles;
}

float getFPSRate() {
  float result = FPS*1.0/lastFPS;
  return result*0.5+1.0*0.5;
}
