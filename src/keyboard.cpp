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

#define M_PI 3.14159265359

#include <SFML/Graphics.hpp> // joystick support

using namespace std;

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define PI 3.14159265

int deadzoneRadius = 20; // This feels really good at 20... I recommend not altering it!
const int playerCount = 4;
const bool jumpingEnabled = true;
const bool lockingEnabled = false;
const bool superEnabled = false;
bool independentMovement = true; // move separately from camera angle after start of movement
int altFrame = 0;
int altMax = 5;

// For pressing start to see who goes first
int playerChosen = -1;

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
bool cameraButton[playerCount];

bool fullscreenKey;
bool levelShadowsKey;
bool nextLevelKey;
bool lastLevelKey;
int lastPause = -1; // Last player to pause

// keep track of angles for dir movement
bool oldUD[playerCount];
bool oldLR[playerCount];
int sinUD[playerCount], cosUD[playerCount], sinLR[playerCount], cosLR[playerCount];

// Not using these much anymore, scrap them?
void setJump(int p, bool b) { if (jumpingEnabled) { if (lockKey[p] && !jumpKey[p] && b) { lockKey[p] = false; } jumpKey[p] = b; } }
void setLock(int p, bool b)  { if (lockingEnabled) { if (jumpKey[p] && !lockKey[p] && b) { jumpKey[p] = false; } lockKey[p] = b; } }
void setSuper(int p, bool b) { if (superEnabled) { superKey[p] = b; } }

// Camera commands
void setCenterCommand(int p, bool b) {
  if (b && !cameraButton[p]) {
    playerCenterCam(p);
  }
  if (cameraButton[p] != b) {
    cameraButton[p] = b;
  }
}
void setLeftCommand(int p, bool b)   { if (b) { playerLeftCam(p);   } }
void setRightCommand(int p, bool b)  { if (b) { playerRightCam(p);  } }
void setUpCommand(int p, bool b)     { if (b) { playerUpCam(p);     } }
void setDownCommand(int p, bool b)   { if (b) { playerDownCam(p);   } }

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
      // Paused or temp title screen

      // title screen
      if (getGameplayFirstRunning()) {
        playerChosen = p;
        for (int i=0; i<4; i++) {
          setCubiorPlayable(i,false);
          resetCubior(i);
        }
        setCubiorPlayable(p,true);
        resetCubior(p);
        setGameplayFirstRunning(false);
      }

      // paused
			if (lastPause == p || lastPause == -1) {
				startGameplay();
        setJustUnpaused(true);
			}

    } else {
      // Gameplay is running!
      if (p<0 || p>3 || getCubiorPlayable(p)) {
        // Can pause if playing
		    stopGameplay();
        setJustPaused(true);
		    lastPause = p;
      } else {
        // Otherwise, join
        playerJoin(p,true);
      }
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
    // merge still necesary to distinguish new vs old input
	}
	// Then get to those commands being issued!
	if (getGameplayRunning()) {
		for (int i = 0; i<playerCount; i++) {
      // First, if joystick has some form of movement (outside deadzone)
      float joystickDist = sqrt((float)(pow((float)joyX[i],2)+pow((float)joyY[i],2)));
		  bool joyMovement = joystickDist > deadzoneRadius;

      // If moving seperately from camera
      if (independentMovement) {
        // And actually, y'know, moving
        if (!directionsPressed[i] && (upKey[i] || downKey[i] || leftKey[i] || rightKey[i] || joyMovement)) {
          // And we weren't previously, and get some keys or joystick movement
          // then update oldAngle and confirm that we're doing something
				  oldAngleY[i] = getCamera(i)->getAngleY();
				  directionsPressed[i] = true;
			  } else if (directionsPressed[i] && !upInput[i] && !downInput[i] && !leftInput[i] && !rightInput[i] && !joyMovement) {
          // Otherwise, if NOTHING is happening, set it to false again
				  directionsPressed[i] = false;
        }
      }

			// First, record if any movement requested
			bool newUD = upInput[i] || downInput[i];
			bool newLR = leftInput[i] || rightInput[i];
			float cameraRad;
      // And set basis for our angles
      if (independentMovement) {
        // base on starting camera angle
        cameraRad = (oldAngleY[i])*PI/180;
      } else {
        // or current camera angle
        cameraRad = (getCamera(i)->getAngleY())*PI/180;
      }

      // NEWDELETEME: cout << "CAMERARAD is " << cameraRad << endl;
			// CAMERA:   up = 0,     right = -PI/2 or 3/2*PI, down = PI,              left = PI/2
			//float joyAngle = atan((float)joyY[i]/joyX[i]) - (joyX[i] < 0 ? M_PI : 0);
			// JOYSTICK: up = -PI/2, right = 0,               down = PI/2 or -3/2*PI, left = -PI
			// to match camera angle though, we must subtract it from PI
			float joyAngle = -atan((float)joyY[i]/joyX[i]) + (joyX[i] < 0 ? M_PI : 0) - PI/2;
			// JOYSTICK: up =  0, right = -PI/2,               down =-PI or PI,       left = PI/2
			// close enough, now use it below!

      // How strong our movement in said direction is
			float ratio;

			// Keys to a direction
			if (upKey[i]||downKey[i]||leftKey[i]||rightKey[i]) {
        
        /***********************
         * KEYBOARD INPUT TIME *
         ***********************/

				if (upKey[i] && !downKey[i]) {
					if (leftKey[i] && !rightKey[i]) {
						joyAngle =  M_PI/4; // up left
					} else if (!leftKey[i] && rightKey[i]) {
						joyAngle = -M_PI/4; // up right
					} else {
						joyAngle = 0; // up straight
					}
				} else if (!upKey[i] && downKey[i]) {
					if (leftKey[i] && !rightKey[i]) {
						joyAngle = M_PI*3/4; // down left
					} else if (!leftKey[i] && rightKey[i]) {
						joyAngle = M_PI*5/4; // down right
					} else {
						joyAngle = M_PI; // down straight
					}
				} else {
					if (leftKey[i] && !rightKey[i]) {
						joyAngle =  M_PI/2; // straight left
					} else if (!leftKey[i] && rightKey[i]) {
						joyAngle = -M_PI/2; // straight right
					}
				}
				// And in any case, ratio is full blast
				ratio = 100;
				// And pressing keys = reset joystick/disable it
				joyX[i]=0;
				joyY[i]=0;

      // No keys pressed? Well then, sounds like it's time for...
			} else {
        
        /***********************
     <===* JOYSTICK INPUT TIME *===}--o
         ***********************/

				// No directional key presses means joystick!
				if (joyX[i] != 0 && joyY[i] != 0) {
					// Then apply joystick ratio vals
					ratio = joystickDist;
          // This ONLY affects joystick control sensitivity
					if (ratio < deadzoneRadius) { 
						ratio = 0;
					}
					// Or it means no buttons...
				} else {
					// No buttons? Don't move!
					ratio = 0;
				}
			}

      /*******************
       * BOTH TIME AGAIN *
       *******************/

      // player to goal angle
      // vs current dir
      // vs angle of last movement if needed

			// Alright! Any movement? Apply it!
			if (ratio != 0) {
        // NEWDELETEME: cout << "joyAngle (" << joyAngle << ") + cameraRad (" << cameraRad << ") = " << joyAngle + cameraRad << endl;
        // NEWDELETEME: cout << "sent moveZ " << -cos(joyAngle+cameraRad)*ratio/5.0 << endl;
        // NEWDELETEME: cout << "sent moveX " << -sin(joyAngle+cameraRad)*ratio/5.0 << endl;
        float toSendZ = -cos(joyAngle+cameraRad)*ratio/5.0;
        float toSendX = -sin(joyAngle+cameraRad)*ratio/5.0;
				getPlayer(i)->moveZ(toSendZ);
        getPlayer(i)->moveX(toSendX);
      }

			// Finally, handle other keys
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
  bool camButtonPressed = false;
	for (int b=0; b<10; b++) {
		if (b == 6) { // Join joinButton
			playerJoin(i,sf::Joystick::isButtonPressed(joystick,b));
		} else if (b == 7) { // Pause pauseButton
			playerPause(i,sf::Joystick::isButtonPressed(joystick,b));
		} else if (b == 4 || b == 9) { // Camera cameraButtons, left bumper/right stick
      // If either is pressed, add that to camButtonPressed
      // so we only send one call later
      camButtonPressed = camButtonPressed || sf::Joystick::isButtonPressed(joystick,b);
    } else { // All unestablish buttons == jumpButton!
			jumpButton[i] = jumpButton[i] || sf::Joystick::isButtonPressed(joystick,b);
    }
	}

  // Cam button stuff
  setCenterCommand(i,camButtonPressed);

	// Convert (for now) joystick to direction buttons
	joyX[i] = sf::Joystick::getAxisPosition(joystick,sf::Joystick::X);
	joyY[i] = sf::Joystick::getAxisPosition(joystick,sf::Joystick::Y);
  // was at 20 for limits, 15 seems fine too (want more flexibility somewhere for turning)
	upButton[i]   = joyY[i] <-15;
	downButton[i] = joyY[i] > 15;
	leftButton[i] = joyX[i] <-15;
	rightButton[i]= joyX[i] > 15;

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

// Quick jump to last level
void lastLevelPressed(bool b) {
	if (!lastLevelKey && b) {
		lastLevel();
	}
	lastLevelKey = b;
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
	case '_': lastLevelPressed(newBool); break;

		// OLD PLAYER 1 CONTROLS 
	case 'c': case 'C': setSuper(0,newBool); break;
	case 'x': case 'X':  setLock(0,newBool); break;
	case 'z': case 'Z':  setJump(0,newBool); break;
		// NEW PLAYER 1
	case 'm': case 'M': setSuper(0,newBool); break;
	case ',': case '<': setLock(0,newBool); break;
	case '.': case '>': setJump(0,newBool); break;
	case ' ':           setJump(0,newBool); break;
    // CAMERA CONTROLS (single player)
  case 9: // press tab to center cam for P1
  case '\\': setCenterCommand(0,newBool); break;

		// PLAYER 2
	case 'w': case 'W':    upKey[1] = newBool; break;
	case 'a': case 'A':  leftKey[1] = newBool; break;
	case 's': case 'S':  downKey[1] = newBool; break;
	case 'd': case 'D': rightKey[1] = newBool; break;
	case 'f': case 'F':    setJump(1,newBool); break;
	case 'g': case 'G':    setLock(1,newBool); break;
	case 'h': case 'H':   setSuper(1,newBool); break;

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

// Give who was chosen as first person to play
int getPlayerChosen() {
  return playerChosen;
}

/* No need now that we use SFML instead of OpenGL for this
void handleJoystickInput(int button, bool b) {
cout << "It was called for " << button << "!\n";
}

void joystickDown(unsigned int button, int x, int y, int z) { handleJoystickInput(button, true); }
*/