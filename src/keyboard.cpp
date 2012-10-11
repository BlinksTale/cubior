/*
 * keyboard.cpp
 * by Brian Handy
 * 1/23/12
 * keyboard for cubior
 */

#include "keyboard.h"
#include <iostream>
#include <math.h>
#include "gameplay.h"

#include <SFML/Graphics.hpp> // joystick support

using namespace std;

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define PI 3.14159265

const int playerCount = 4;
const bool jumpingEnabled = true;
const bool lockingEnabled = false;
const bool superEnabled = false;
int altFrame = 0;
int altMax = 5;

bool directionsPressed[playerCount]; // whether movement has started for this player
int oldAngleY[playerCount]; // represents cam angle for player when keys first pressed

// For all input merged
bool upInput[playerCount];
bool downInput[playerCount];
bool leftInput[playerCount];
bool rightInput[playerCount];
bool jumpInput[playerCount];

// For Keyboards
bool upKey[playerCount];
bool downKey[playerCount];
bool leftKey[playerCount];
bool rightKey[playerCount];
bool jumpKey[playerCount];
bool lockKey[playerCount];
bool superKey[playerCount];
bool pauseKey[playerCount];
bool joinKey[playerCount];

// For Joysticks
int joyX[playerCount];
int joyY[playerCount];
bool upButton[playerCount];
bool downButton[playerCount];
bool leftButton[playerCount];
bool rightButton[playerCount];
bool jumpButton[playerCount];
bool lockButton[playerCount];
bool superButton[playerCount];
bool pauseButton[playerCount];
bool joinButton[playerCount];

bool fullscreenKey;
bool levelShadowsKey;
bool nextLevelKey;
int lastPause = -1; // Last player to pause

// keep track of angles for dir movement
bool oldUD[playerCount];
bool oldLR[playerCount];
int sinUD[playerCount], cosUD[playerCount], sinLR[playerCount], cosLR[playerCount];
    
// Not using these much anymore, scrap them?
void setJump(int p, bool b) { if (jumpingEnabled) { if (lockKey[p] && !jumpKey[p] && b) { lockKey[p] = false; } jumpKey[p] = b; } }
void setLock(int p, bool b)  { if (lockingEnabled) { if (jumpKey[p] && !lockKey[p] && b) { jumpKey[p] = false; } lockKey[p] = b; } }
void setSuper(int p, bool b) { if (superEnabled) { superKey[p] = b; } }

// Pause fullscreen can be anyone, actually
void playerFullscreen(bool newBool) {
  if (!fullscreenKey && newBool) { // newly pressing Enter
    switchFullscreen();
  }
  fullscreenKey = newBool;
}

// Level shadows can be anyone too
void playerLevelShadows(bool newBool) {
  if (!levelShadowsKey && newBool) { // newly pressing Enter
    switchLevelShadows();
  }
  levelShadowsKey = newBool;
}

// Pause tied to player who paused
void playerPause(int p, bool newBool) {
  if (!pauseKey[p] && newBool) { // newly pressing Enter
    if (!getGameplayRunning()) {
      if (lastPause == p || lastPause == -1) {
        startGameplay();
      }
    } else {
      stopGameplay();
      lastPause = p;
    }
  }
  pauseKey[p] = newBool;
}

void playerJoin(int p, bool newBool) {
  if (!joinKey[p] && newBool) { setCubiorPlayable(p,!getCubiorPlayable(p)); }
  joinKey[p] = newBool;
}

int getLastPause() { return lastPause; }

// Once per loop, send off the commands from these inputs
void sendCommands() {
  // joystick support, ALWAYS update its input
  sf::Joystick::update(); 

  // Update each joystick's info before sending commands
  for (int i=0; i < 4; i++) {
	  joystickCommands(i);
      mergeInput(i); // then stick it and key inputs together
  }
  // Then get to those commands being issued!
  if (getGameplayRunning()) {
    for (int i = 0; i<playerCount; i++) {
      if (!directionsPressed[i] && (upInput[i] || downInput[i] || leftInput[i] || rightInput[i])) {
        oldAngleY[i] = getCamera(i)->getAngleY();
        directionsPressed[i] = true;
      } else if (directionsPressed[i] && !upInput[i] && !downInput[i] && !leftInput[i] && !rightInput[i]) {
        directionsPressed[i] = false;
      }
      // First, record if any movement requested
      bool newUD = upInput[i] || downInput[i];
      bool newLR = leftInput[i] || rightInput[i];
      
      // Then set movement angles based on cam angle if that's true
      //if (newUD && !oldUD[i]) {
        sinUD[i] = ceil(sin((int)(/*getCamera(i)->getAngleY()*/oldAngleY[i])*PI/180)-0.5);
        cosUD[i] = ceil(cos((int)(/*getCamera(i)->getAngleY()*/oldAngleY[i])*PI/180)-0.5);
      //}
      //if (newLR && !oldLR[i]) {
        sinLR[i] = ceil(sin((360-(int)(/*getCamera(i)->getAngleY()*/oldAngleY[i]))*PI/180)-0.5);
        cosLR[i] = ceil(cos((360-(int)(/*getCamera(i)->getAngleY()*/oldAngleY[i]))*PI/180)-0.5);
      //}
      
      if (upInput[i]) {
		float upRatio = 1.0;
        // careful! If you cast as int before multiplying by ten, it rounds to zero
		if (upButton[i]) { upRatio = (joyY[i]-50)/-100.0; }
        getPlayer(i)->moveZ((int)(cosUD[i]*(-10)*upRatio));
        getPlayer(i)->moveX((int)(sinUD[i]*(-10)*upRatio));
      }
      if (downInput[i]) {
		float downRatio = 1.0;
		if (downButton[i]) { downRatio = (joyY[i]+50)/100.0; }
        getPlayer(i)->moveZ((int)(cosUD[i]*( 10)*downRatio));
        getPlayer(i)->moveX((int)(sinUD[i]*( 10)*downRatio));
      }
      if (leftInput[i]) {
		float leftRatio = 1.0;
		if (leftButton[i]) { leftRatio = (joyX[i]-50)/-100.0; }
        getPlayer(i)->moveZ((int)(sinLR[i]*(-10)*leftRatio));
        getPlayer(i)->moveX((int)(cosLR[i]*(-10)*leftRatio));
      }
      if (rightInput[i]) {
		float rightRatio = 1.0;
		if (rightButton[i]) { rightRatio = (joyX[i]+50)/100.0; }
        getPlayer(i)->moveZ((int)(sinLR[i]*( 10)*rightRatio));
        getPlayer(i)->moveX((int)(cosLR[i]*( 10)*rightRatio));
      }
      getPlayer(i)->jump(jumpInput[i]);
      getPlayer(i)->setLock(lockKey[i]);
      getPlayer(i)->setInvincibility(superKey[i]);
      
      // Then make sure oldMovement is up to date
      if (oldUD[i] != newUD) {
        oldUD[i] = newUD;
      }
      if (oldLR[i] != newLR) {
        oldLR[i] = newLR;
      }
    }
  }
}

// Given a player num, returns joystick num
int joystickNum(int i) {
  // Please note... I don't understand the order either,
  // but until I make player order based on joining order,
  // this works.
  switch(i) {
	case 0: // Player 1
	  return 1;
	case 1: // Player 2
	  return 2;
	case 2: // Player 3
	  return 0;
	case 3: // Player 4
	  return 3;
	default:
	  return 1;
  }
}

// Figure out all joystick input translations to key presses, for now
void joystickCommands(int i) {
  int joystick = joystickNum(i);
  // Accept any not-start-or-select button for jumping
  jumpButton[i] = 0;
  for (int b=0; b<8; b++) {
	if (b != 6 && b != 7) {
	  jumpButton[i] = jumpButton[i] || sf::Joystick::isButtonPressed(joystick,b);
	} else if (b == 6) { // Join
	  playerJoin(i,sf::Joystick::isButtonPressed(joystick,b));
	} else if (b == 7) { // Pause
	  playerPause(i,sf::Joystick::isButtonPressed(joystick,b));
	}
  }

  // Convert (for now) joystick to direction buttons
  joyX[i] = sf::Joystick::getAxisPosition(joystick,sf::Joystick::X);
  joyY[i] = sf::Joystick::getAxisPosition(joystick,sf::Joystick::Y);
  upButton[i]   = joyY[i] <-25;
  downButton[i] = joyY[i] > 25;
  leftButton[i] = joyX[i] <-25;
  rightButton[i]= joyX[i] > 25;

}

// Combine buttons and keys under one input system
void mergeInput(int i) {
	// Only covering stuff really used here
	// join and pause handled seperately
	upInput[i] = upButton[i] || upKey[i];
	downInput[i] = downButton[i] || downKey[i];
	leftInput[i] = leftButton[i] || leftKey[i];
	rightInput[i] = rightButton[i] || rightKey[i];
	jumpInput[i] = jumpButton[i] || jumpKey[i];
}

// Quick jump to next level
void nextLevelPressed(bool b) {
  if (!nextLevelKey && b) {
    nextLevel();
  }
  nextLevelKey = b;
}
// Handle keyboard input.
void handleInput(unsigned char key, bool newBool) {
  switch(key) {
    // SYSTEM CONTROLS
    case '0': playerFullscreen(newBool); break;//enableGoodCollision(); break;
    case '9': playerLevelShadows(newBool); break;
    //case '9': disableGoodCollision(); break;
    case 13 : // Press Enter to pause from p1
    case '5': playerPause(0,newBool); break;
    case '6': playerPause(1,newBool); break;
    case '7': playerPause(2,newBool); break;
    case '8': playerPause(3,newBool); break;
    case '1': playerJoin(0,newBool); break;
    case '2': playerJoin(1,newBool); break;
    case '3': playerJoin(2,newBool); break;
    case '4': playerJoin(3,newBool); break;
    // Shift + '=' to jump ahead a level
    case '+': nextLevelPressed(newBool); break;
    
    // OLD PLAYER 1 CONTROLS 
    case 'c': case 'C': setSuper(0,newBool); break;
    case 'x': case 'X':  setLock(0,newBool); break;
    case 'z': case 'Z':  setJump(0,newBool); break;
    // NEW PLAYER 1
    case 'm': case 'M': setSuper(0,newBool); break;
    case ',': case '<': setLock(0,newBool); break;
    case '.': case '>': setJump(0,newBool); break;
    case ' ':           setJump(0,newBool); break;

    // PLAYER 2
    case 'w': case 'W':   upKey[1] = newBool; break;
    case 'a': case 'A':    leftKey[1] = newBool; break;
    case 's': case 'S': downKey[1] = newBool; break;
    case 'd': case 'D':  rightKey[1] = newBool; break;
    case 'f': case 'F':   setJump(1,newBool); break;
    case 'g': case 'G':   setLock(1,newBool); break;
    case 'h': case 'H': setSuper(1,newBool); break;
    
    // PLAYER 3
    case 'i': case 'I':       upKey[2] = newBool; break;
    case 'u': case 'U':    leftKey[2] = newBool; break;
    case 'o': case 'O': downKey[2] = newBool; break;
    case 'p': case 'P':  rightKey[2] = newBool; break;
    case '[':  case '{':  setJump(2,newBool); break;

    // PLAYER 4
    case 'k': case 'K':    upKey[3] = newBool; break;
    case 'j': case 'J':    leftKey[3] = newBool; break;
    case 'l': case 'L': downKey[3] = newBool; break;
    case ';': case ':':  rightKey[3] = newBool; break;
    case '\'':  case '"':  setJump(3,newBool); break;
/*
    case '1': case '!':
      playerCount = 1;
      break;
    case '2': case '@':
      playerCount = 2;
      break;
*/
    // QUIT
    case 27: exit(0); break;
    default: break;
  }
}

void inputDown(unsigned char key, int x, int y) { handleInput(key, true); }
void inputUp(unsigned char key, int x, int y) { handleInput(key, false); }

// nonASCII keys go here
void handleSpecialInput(int key, bool newBool) {
  switch(key) {
    case GLUT_KEY_UP:          upKey[0] = newBool; break; 
    case GLUT_KEY_DOWN: downKey[0] = newBool; break;
    case GLUT_KEY_LEFT:       leftKey[0] = newBool; break;
    case GLUT_KEY_RIGHT:   rightKey[0] = newBool; break;
    default: break;
  }
}

void specialInputDown(int key, int x, int y) { handleSpecialInput(key, true); }
void specialInputUp(int key, int x, int y)   { handleSpecialInput(key, false); }

/* No need now that we use SFML instead of OpenGL for this
void handleJoystickInput(int button, bool b) {
  cout << "It was called for " << button << "!\n";
}

void joystickDown(unsigned int button, int x, int y, int z) { handleJoystickInput(button, true); }
*/