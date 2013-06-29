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
#include "music.h"

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
bool independentMovementDefault = false;
bool independentMovement[] = { // move separately from camera angle after start of movement
  independentMovementDefault,
  independentMovementDefault,
  independentMovementDefault,
  independentMovementDefault}; 
int altFrame = 0;
int altMax = 5;

// temp var until we can detect PS3 controllers
#define PS3Controls true
#define cameraButtonDisabled true
/* Change which 360 buttons we use on Mac vs PC */
/* (they read the controller differently) */
#ifdef __APPLE_CC__
#ifdef PS3Controls
// DS3 (PS3) controller support here
int joinButtonNum = 0; // select
int pauseButtonNum = 3; // start
// These two disabled for now since camera button causes problems
int cameraBumperButtonNum = -1;//10; // left bumper
int cameraStickButtonNum = -1;//2; // right stick click down
#else
// 360 Controller support
int joinButtonNum = 11 - 1;
int pauseButtonNum = 10 - 1;
int cameraBumperButtonNum = 14 - 1;
int cameraStickButtonNum = 13 - 1;
#endif
#else
// 360 controller on Windows
int joinButtonNum = 6;
int pauseButtonNum = 7;
int cameraBumperButtonNum = 4;
int cameraStickButtonNum = 9;
#endif

// For pressing start to see who goes first
int controlsChosen = -1;
int controlsPlayer[] = {-1, -1, -1, -1}; // Controls x move player y

bool directionsPressed[playerCount]; // whether movement has started for this player
int oldAngleY[playerCount]; // represents cam angle for player when keys first pressed

// For all input merged
bool upInput[playerCount];
bool downInput[playerCount];
bool leftInput[playerCount];
bool rightInput[playerCount];
bool jumpInput[playerCount];

// For menu navigation
bool lastUpInput[playerCount];
bool lastDownInput[playerCount];
bool lastJumpInput[playerCount];

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
bool secondaryJoyLeft[playerCount];
bool secondaryJoyRight[playerCount];
bool secondaryJoyUp[playerCount];
bool secondaryJoyDown[playerCount];

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

// Did we have a joystick last we checked?
bool joystickRecentlyConnected = false;

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
  // Handle universal pauses and regular pauses
  if ((!pauseKey[p] || p == -1) && newBool) { // newly pressing Enter
		if (!getGameplayRunning()) {
      // title screen
      // choose who pressed start and set them as playable
      if (getGameplayFirstRunning()) {
        int o = 0;
        controlsPlayer[p] = o;
        controlsChosen = o;
        for (int i=0; i<4; i++) {
          setCubiorPlayable(i,false);
          resetCubior(i);
        }
        setCubiorPlayable(o,true);
        resetCubior(o);
        setGameplayFirstRunning(false);
        setMenu(o,1);
      // Otherwise, paused and selecting option
      } else if (lastPause == p || lastPause == -1) {
        int o = controlsPlayer[p];
				chooseOption(o);
			}
    // Or handle all un-pause actions
    } else {
      int o = controlsPlayer[p];
      // Gameplay is running!
      if (p<0 || p>3 || getCubiorPlayable(o)) {
        // Can pause if playing
        setMenu(o,2); // pause menu
		    stopGameplay(o);
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

void playerJoin(int k, bool newBool) {
	if (!joinKey[k] && newBool) {
    if (controlsPlayer[k] == -1) {
      if (getCubiorsPlayable() > 0) { 
        int p = getNewPlayer();
        controlsPlayer[k] = p;
        setCubiorPlayable(p,!getCubiorPlayable(p));
      }
    } else if (getCubiorsPlayable() > 1) {
      setCubiorPlayable(controlsPlayer[k],false);
      resetControlsPlayer(controlsPlayer[k]);
    }
  }
	joinKey[k] = newBool;
}

void resetControlsPlayer(int k) {
  for (int i=0; i<4; i++) {
    if (controlsPlayer[i] == k) {
      controlsPlayer[i] = -1;
    }
  }
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
      if (independentMovement[i]) {
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
      if (independentMovement[i]) {
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
  // Gameplay not running? Send pause commands!
	} else {
		for (int i = 0; i<playerCount; i++) {
      if (  upInput[i] &&   !lastUpInput[i]) { prevOption(i); }
      if (downInput[i] && !lastDownInput[i]) { nextOption(i); }
      if (jumpInput[i] && !lastJumpInput[i]) { chooseOption(i); }
      // And update old vars
      lastUpInput[i] = upInput[i];
      lastDownInput[i] = downInput[i];
      lastJumpInput[i] = jumpInput[i];
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
  
  // Read in as many buttons as the joystick has
	for (int b=0; b<sf::Joystick::getButtonCount(joystick); b++) {
        // Println joystick controls
        //if (sf::Joystick::isButtonPressed(joystick, b)) {
        //    cout << "Key " << b << " was pressed" << endl;
        //}
		if (b == joinButtonNum) { // Join joinButton
			playerJoin(i,sf::Joystick::isButtonPressed(joystick,b));
		} else if (b == pauseButtonNum) { // Pause pauseButton
			playerPause(i,sf::Joystick::isButtonPressed(joystick,b));
		} else if (b == cameraBumperButtonNum || b == cameraStickButtonNum) { // cameraButtons, left bumper/right stick
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
  
  // Right joystick X
  #ifdef PS3Controls
    int newSecondaryX = sf::Joystick::getAxisPosition(joystick,sf::Joystick::Z);
  #else
    int newSecondaryX = sf::Joystick::getAxisPosition(joystick,sf::Joystick::U);
  #endif
    
  // Right joystick Y
  #ifdef __APPLE_CC__
  #ifdef PS3Controls
    int newSecondaryY = sf::Joystick::getAxisPosition(joystick,sf::Joystick::R);
  #else
  int newSecondaryY = sf::Joystick::getAxisPosition(joystick,sf::Joystick::V);
  #endif
  #else
  int newSecondaryY = sf::Joystick::getAxisPosition(joystick,sf::Joystick::R);
  #endif
  int secondaryTrigger = 50;
  bool newSecondaryLeft  = newSecondaryX <-secondaryTrigger;
  bool newSecondaryRight = newSecondaryX > secondaryTrigger;
  bool newSecondaryUp    = newSecondaryY <-secondaryTrigger;
  bool newSecondaryDown  = newSecondaryY > secondaryTrigger;
  // Pass the trigger and have not done this last time to activate the call
  if (newSecondaryLeft != secondaryJoyLeft[i]) { 
    setLeftCommand(i,newSecondaryLeft);
    secondaryJoyLeft[i]  = newSecondaryLeft;
  }
  if (newSecondaryRight != secondaryJoyRight[i]) { 
    setRightCommand(i,newSecondaryRight);
    secondaryJoyRight[i] = newSecondaryRight;
  }
  if (newSecondaryUp != secondaryJoyUp[i]) {
    setUpCommand(i,newSecondaryUp); 
    secondaryJoyUp[i] = newSecondaryUp;
  }
  if (newSecondaryDown != secondaryJoyDown[i]) { 
    setDownCommand(i,newSecondaryDown);
    secondaryJoyDown[i] = newSecondaryDown;
  }
}

// Return if any joystick connected
bool joystickConnected() {
  bool result = false;
  // check for all four
  for (int i=0; i<4; i++) {
    result = result || sf::Joystick::isConnected(joystickNum(i));
  }
  joystickRecentlyConnected = result;
  // if any connected, return true
  return result;
}

// Gives what state was as of last time checking
bool joystickWasConnected() {
  return joystickRecentlyConnected;
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
  int a = controlsPlayer[0];
  int b = controlsPlayer[1];
  int c = controlsPlayer[2];
  int d = controlsPlayer[3];
	switch(key) {
		// SYSTEM CONTROLS
	case '0': playerFullscreen(newBool); break;//enableGoodCollision(); break;
	case '9': playerLevelShadows(newBool); break;
		//case '9': disableGoodCollision(); break;
	case 13 : // Press Enter to pause from p1
	case 27 : // Or Esc
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
  case '-': decreaseMusicVolume(); break;
  case '=': increaseMusicVolume(); break;

		// OLD PLAYER 1 CONTROLS 
	case 'c': case 'C': setSuper(a,newBool); break;
	case 'x': case 'X':  setLock(a,newBool); break;
	case 'z': case 'Z':  setJump(a,newBool); break;
		// NEW PLAYER 1
	case 'm': case 'M': setSuper(a,newBool); break;
	case ',': case '<': setLock(a,newBool); break;
	case '.': case '>': setJump(a,newBool); break;
	case ' ':           setJump(a,newBool); break;
    // CAMERA CONTROLS (single player)
  case 9: // press tab to center cam for P1
  case '\\': setCenterCommand(a,newBool); break;

		// PLAYER 2
	case 'w': case 'W':    
    if (getCubiorPlayable(b)) {
      upKey[b] = newBool;
    } else {
      setUpCommand(a,newBool);
    }
    break;
	case 'a': case 'A':
    if (getCubiorPlayable(b)) {
      leftKey[b] = newBool;
    } else {
      setLeftCommand(a,newBool);
    }
    break;
	case 's': case 'S':
    if (getCubiorPlayable(b)) {
      downKey[b] = newBool;
    } else {
      setDownCommand(a,newBool);
    }
    break;
	case 'd': case 'D':
    if (getCubiorPlayable(b)) {
      rightKey[b] = newBool;
    } else {
      setRightCommand(a,newBool);
    }
    break;
	case 'f': case 'F':    setJump(b,newBool); break;
	case 'g': case 'G':    setLock(b,newBool); break;
	case 'h': case 'H':   setSuper(b,newBool); break;

		// PLAYER 3
	case 'i': case 'I':       upKey[c] = newBool; break;
	case 'u': case 'U':    leftKey[c] = newBool; break;
	case 'o': case 'O': downKey[c] = newBool; break;
	case 'p': case 'P':  rightKey[c] = newBool; break;
	case '[':  case '{':  setJump(c,newBool); break;

		// PLAYER 4
	case 'k': case 'K':    upKey[d] = newBool; break;
	case 'j': case 'J':    leftKey[d] = newBool; break;
	case 'l': case 'L': downKey[d] = newBool; break;
	case ';': case ':':  rightKey[d] = newBool; break;
	case '\'':  case '"':  setJump(d,newBool); break;
		/*
		case '1': case '!':
		playerCount = 1;
		break;
		case '2': case '@':
		playerCount = 2;
		break;
		*/
	default: break;
	}
}

void inputDown(unsigned char key, int x, int y) { handleInput(key, true); }
void inputUp(unsigned char key, int x, int y) { handleInput(key, false); }

// nonASCII keys go here
void handleSpecialInput(int key, bool newBool) {
  int a = controlsPlayer[0];
	switch(key) {
	case GLUT_KEY_UP:          upKey[a] = newBool; break; 
	case GLUT_KEY_DOWN:      downKey[a] = newBool; break;
	case GLUT_KEY_LEFT:      leftKey[a] = newBool; break;
	case GLUT_KEY_RIGHT:    rightKey[a] = newBool; break;
	default: break;
	}
}

void specialInputDown(int key, int x, int y) { handleSpecialInput(key, true); }
void specialInputUp(int key, int x, int y)   { handleSpecialInput(key, false); }

// Give who was chosen as first person to play
int getPlayerChosen() {
  return controlsChosen;
}

/* No need now that we use SFML instead of OpenGL for this
void handleJoystickInput(int button, bool b) {
cout << "It was called for " << button << "!\n";
}

void joystickDown(unsigned int button, int x, int y, int z) { handleJoystickInput(button, true); }
*/

bool getIndependentMovement(int i) {
  return independentMovement[i];
}
void toggleIndependentMovement(int i) {
  independentMovement[i] = !independentMovement[i];
}