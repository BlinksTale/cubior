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

#define Unknown -1
#define Xbox 0
#define PS3 1
#define GameCube 2
#define WiiU 3

int deadzoneRadius = 20; // This feels really good at 20... I recommend not altering it!
const int localPlayerCount = 4;
const int joystickCount = 16; // maxJoystick / joystickMax / etc
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

// Where all our buttons will be assigned
int joystickType[joystickCount];
int joystickButtonCount[joystickCount];
int joinButtonNum[joystickCount];
int pauseButtonNum[joystickCount];
int cameraBumperButtonNum[joystickCount];
int cameraStickButtonNum[joystickCount];

// For pressing start to see who goes first
int controlsChosen = -1;
int controlsPlayer[joystickCount];
// = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}; // Controls x move player y
bool keyboard[localPlayerCount];

bool directionsPressed[localPlayerCount]; // whether movement has started for this player
int oldAngleY[localPlayerCount]; // represents cam angle for player when keys first pressed

// For all input merged
bool upInput[localPlayerCount];
bool downInput[localPlayerCount];
bool leftInput[localPlayerCount];
bool rightInput[localPlayerCount];
bool jumpInput[localPlayerCount];

// For menu navigation
bool lastUpInput[localPlayerCount];
bool lastDownInput[localPlayerCount];
bool lastLeftInput[localPlayerCount];
bool lastRightInput[localPlayerCount];
bool lastJumpInput[localPlayerCount];

// For Keyboards
bool upKey[localPlayerCount];
bool downKey[localPlayerCount];
bool leftKey[localPlayerCount];
bool rightKey[localPlayerCount];
bool jumpKey[localPlayerCount];
bool lockKey[localPlayerCount];
bool superKey[localPlayerCount];
bool pauseKey[localPlayerCount];
bool joinKey[localPlayerCount];

// For Joysticks
int joyX[localPlayerCount];
int joyY[localPlayerCount+1]; // I am concerned as to why this is a fix, but it works. Fixes p4 issue.
bool upButton[localPlayerCount];
bool downButton[localPlayerCount];
bool leftButton[localPlayerCount];
bool rightButton[localPlayerCount];
bool jumpButton[localPlayerCount];
bool lockButton[localPlayerCount];
bool superButton[localPlayerCount];
bool pauseButton[localPlayerCount];
bool joinButton[localPlayerCount];
bool cameraButton[localPlayerCount];
bool secondaryJoyLeft[localPlayerCount];
bool secondaryJoyRight[localPlayerCount];
bool secondaryJoyUp[localPlayerCount];
bool secondaryJoyDown[localPlayerCount];

bool fullscreenKey;
bool levelShadowsKey;
bool nextLevelKey;
bool lastLevelKey;
int lastPause = -1; // Last player to pause

// keep track of angles for dir movement
bool oldUD[localPlayerCount];
bool oldLR[localPlayerCount];
int sinUD[localPlayerCount], cosUD[localPlayerCount], sinLR[localPlayerCount], cosLR[localPlayerCount];

// Initialization
void keyboardInit() {
  for (int i=0; i<joystickCount; i++) {
    controlsPlayer[i] = -1;
  }
  
  sf::Joystick::update();
  // Check first for joysticks joining the fray
  for (int i=0; i < joystickCount; i++) {
      setJoystickControls(i);
  }
}

void setJoystickControls(int i) {
    int buttonCount = sf::Joystick::getButtonCount(i);
    if (joystickButtonCount[i] != buttonCount) {
    joystickButtonCount[i] = buttonCount;
#ifdef __APPLE_CC__
    if (buttonCount == 19) { // PS3
        joystickType[i] = PS3;
        // PS3 controller on Mac (buttonCount == 19, figure it out in joystickCommands?)
        joinButtonNum[i] = 0; // select
        pauseButtonNum[i] = 3; // start
        // These two disabled for now since camera button causes problems
        cameraBumperButtonNum[i] = -1;//10; // left bumper
        cameraStickButtonNum[i] = -1;//2; // right stick click down
    } else if (buttonCount == 16) { // Gamecube
        joystickType[i] = GameCube;
        // GameCube controller on Mac (buttonCount == 16)
        joinButtonNum[i] = 3;//3 is Y button;
        pauseButtonNum[i] = 9;//9 is start;
        cameraBumperButtonNum[i] = -1;//
        cameraStickButtonNum[i] = -1;//
    } else if (buttonCount == 28) {
        joystickType[i] = WiiU;
        // 360 controller on Mac
        joinButtonNum[i] = 7;
        pauseButtonNum[i] = 6;
        cameraBumperButtonNum[i] = -1;//14 - 1;
        cameraStickButtonNum[i] = -1;//13 - 1;
    } else if (buttonCount == 20) {
        joystickType[i] = Xbox;
        // 360 controller on Mac
        joinButtonNum[i] = 11 - 2;
        pauseButtonNum[i] = 10 - 2;
        cameraBumperButtonNum[i] = -1;//14 - 1;
        cameraStickButtonNum[i] = -1;//13 - 1;
    } else {
        joystickType[i] = Unknown;
        // 360 controller on Mac
        joinButtonNum[i] = 4;
        pauseButtonNum[i] = 3;
        cameraBumperButtonNum[i] = -1;
        cameraStickButtonNum[i] = -1;
    }
#else
    // 360 controller on Windows
    joinButtonNum[i] = 6;
    pauseButtonNum[i] = 7;
    cameraBumperButtonNum[i] = -1;//4;
    cameraStickButtonNum[i] = -1;//9;
#endif
}
}

// Not using these much anymore, scrap them?
void setJump(int p, bool b) { if (jumpingEnabled) { if (lockKey[p] && !jumpKey[p] && b) { lockKey[p] = false; } jumpKey[p] = b; } }
void setLock(int p, bool b)  { if (lockingEnabled) { if (jumpKey[p] && !lockKey[p] && b) { jumpKey[p] = false; } lockKey[p] = b; } }
void setSuper(int p, bool b) { if (superEnabled) { superKey[p] = b; } }

// Did we have a joystick last we checked?
bool joystickRecentlyConnected = false;

// Camera commands
void setCenterCommand(int p, bool b) {
  /* TEMPORARILY DISABLED
  if (b && !cameraButton[p]) {
    playerCenterCam(p);
  }
  if (cameraButton[p] != b) {
    cameraButton[p] = b;
  }*/
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
void playerPause(int p, bool newBool, bool keyboardBool) {
  // If not a universal pause, set us to started
  if (newBool) {
    setStarted(p != -1);
  }
    
  // Handle universal pauses and regular pauses
  if ((!pauseKey[p] || p == -1) && newBool) { // newly pressing Enter
    if (!getGameplayRunning()) {
      // title screen
      if (getGameplayFirstRunning()) {
        // first, remove all local players since joining from title screen
        //resetLocalCubiors(); // unneccessary
        for (int i=0; i<cubiorCount; i++) {
          setLocalCubiorPlaying(i,false);
          resetCubior(i);
          controlsPlayer[i] = -1;
        }
        // then choose who pressed start and set them as playable
        int o = getNewLocalPlayer();//0;
        controlsPlayer[p] = o;
        keyboard[o] = keyboardBool; // whether on keyboard or not (joystick)
        controlsChosen = o;
        setCubiorPlaying(o,true);
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
      if (p<0 || p>joystickCount || getCubiorPlaying(o)) {
        // Can pause if playing
        setMenu(o,2); // pause menu
		    stopGameplay(o);
        setJustPaused(true);
		    lastPause = p;
      } else {
        //cout << "Joining " << p << endl;
        // Otherwise, join
        playerDirectJoin(p, keyboardBool);
      }
		}
	}
	pauseKey[p] = newBool;
}

void playerJoin(int k, bool newBool, bool keyboardBool) {
  if (!joinKey[k] && newBool) {
    playerDirectJoin(k, keyboardBool);
  }
  joinKey[k] = newBool;
}

void playerDirectJoin(int k, bool keyboardBool) {
    if (controlsPlayer[k] == -1) {
      if (getCubiorsPlaying() < localPlayerCount) { 
        int p = getNewLocalPlayer();
        controlsPlayer[k] = p;
        keyboard[p] = keyboardBool;
        setCubiorPlaying(p,!getCubiorPlaying(p));
      }
    } else if (getLocalCubiorsPlaying() > 1) {
      setCubiorPlaying(controlsPlayer[k],false);
      controlsPlayer[k] = -1;
    }
}
void resetControlsPlayer(int k) {
  for (int i=0; i<joystickCount; i++) {
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

  // Check first for joysticks joining the fray
  for (int i=0; i < joystickCount; i++) {
    // Make sure controls are up to date
    setJoystickControls(i);
      
    // Then send commands
    if (joystickButtonCount[i] > 0 && joystickButtonCount[i] < 200) {
      joystickCommands(i);
    }
  }
	// Then update each active joystick's info before sending commands
  for (int i=0; i < localPlayerCount; i++) {
		mergeInput(i); // then stick it and key inputs together
    // merge still necesary to distinguish new vs old input
	}
	// Then get to those commands being issued!
	if (getGameplayRunning()) {
		for (int i = 0; i<localPlayerCount; i++) {
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
      for (int i = 0; i<localPlayerCount; i++) {
        if (  upInput[i] &&   !lastUpInput[i]) { prevOption(i); }
        if (downInput[i] && !lastDownInput[i]) { nextOption(i); }
        if ( leftInput[i] &&  !lastLeftInput[i]) { prevFocus(i); }
        if (rightInput[i] && !lastRightInput[i]) { nextFocus(i); }
        if (jumpInput[i] && !lastJumpInput[i]) { chooseOption(i); }
        // And update old vars
        lastUpInput[i] = upInput[i];
        lastDownInput[i] = downInput[i];
        lastLeftInput[i] = leftInput[i];
        lastRightInput[i] = rightInput[i];
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

// Check all joysticks in case a new player is joining
void joystickAdditions(int joystick) {
  // Check all player controls in case we're in use
  bool inUse = false;
  for (int p=0; p<localPlayerCount; p++) {
    if (controlsPlayer[p] == joystick) {
      inUse = true;
    }
  }
  // Not in use? Try to be! Listen for a start key
  if (!inUse) {
    //cout << "Join joystick " << joystick << " is " << sf::Joystick::isButtonPressed(joystick,pauseButtonNum) << endl;
	  playerPause(joystick,sf::Joystick::isButtonPressed(joystick,pauseButtonNum[joystick]), false);
  }
}

// Figure out all joystick input translations to key presses, for now
void joystickCommands(int joystick) {
	int i = controlsPlayer[joystick];//joystickNum(i);//
    bool camButtonPressed = false;
  
  // Accept any not-start-or-select button for jumping
	jumpButton[i] = 0;
    // Read in as many buttons as the joystick has
    int buttonCount = sf::Joystick::getButtonCount(joystick);
	for (int b=0; b<buttonCount; b++) {
        //cout << "Button count " << buttonCount << endl;
		if (b == joinButtonNum[joystick]) { // Join joinButton
			playerJoin(joystick,sf::Joystick::isButtonPressed(joystick,b), false);
		} else if (b == pauseButtonNum[joystick]) { // Pause pauseButton
			playerPause(joystick,sf::Joystick::isButtonPressed(joystick,b), false);
		} else if (b == cameraBumperButtonNum[joystick] || b == cameraStickButtonNum[joystick]) { // cameraButtons, left bumper/right stick
            // If either is pressed, add that to camButtonPressed
            // so we only send one call later
            camButtonPressed = camButtonPressed || sf::Joystick::isButtonPressed(joystick,b);
        } else { // All unestablish buttons == jumpButton!
			jumpButton[i] = jumpButton[i] || sf::Joystick::isButtonPressed(joystick,b);
        }
        //if (sf::Joystick::isButtonPressed(joystick,b)) {
        //    cout << "Pressed button " << b << endl;
        //}
	}
  if (i >= 0 && i < localPlayerCount) {
  // Cam button stuff
  setCenterCommand(i,camButtonPressed);

	// Convert (for now) joystick to direction buttons
	joyX[i] = sf::Joystick::getAxisPosition(joystick,sf::Joystick::X);
	joyY[i] = sf::Joystick::getAxisPosition(joystick,sf::Joystick::Y);
    if (joystickType[joystick] == WiiU) {
        joyX[i] *= 2;
        joyY[i] *= 2;
        if (joyX[i] > 100) { joyX[i] = 100; }
        if (joyX[i] <-100) { joyX[i] =-100; }
        if (joyY[i] <-100) { joyY[i] =-100; }
        if (joyY[i] > 100) { joyY[i] = 100; }
    }
  // was at 20 for limits, 15 seems fine too (want more flexibility somewhere for turning)
	upButton[i]   = joyY[i] <-15;
	downButton[i] = joyY[i] > 15;
	leftButton[i] = joyX[i] <-15;
	rightButton[i]= joyX[i] > 15;
  
  // Right joystick X
    int newSecondaryX;
    if (joystickType[joystick] == PS3) {
        newSecondaryX = sf::Joystick::getAxisPosition(joystick,sf::Joystick::Z);
    } else if (joystickType[joystick] == GameCube) {
        newSecondaryX = sf::Joystick::getAxisPosition(joystick,sf::Joystick::R);
    } else if (joystickType[joystick] == WiiU) {
        newSecondaryX = sf::Joystick::getAxisPosition(joystick,sf::Joystick::Z);
    } else {
        newSecondaryX = sf::Joystick::getAxisPosition(joystick,sf::Joystick::U);
    }

  // Right joystick Y
  #ifdef __APPLE_CC__
  int newSecondaryY;
    if (joystickType[joystick] == PS3) {
        newSecondaryY = sf::Joystick::getAxisPosition(joystick,sf::Joystick::R);
    } else if (joystickType[joystick] == GameCube) {
        newSecondaryY = sf::Joystick::getAxisPosition(joystick,sf::Joystick::Z);
    } else if (joystickType[joystick] == WiiU) {
        newSecondaryY = sf::Joystick::getAxisPosition(joystick,sf::Joystick::U);
  } else {
      newSecondaryY = sf::Joystick::getAxisPosition(joystick,sf::Joystick::V);
  }
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
}


// Return if any joystick connected
bool joystickConnected() {
  bool result = false;
  // check for all four
    //for (int i=0; i<localPlayerCount; i++) {
    //    result = result || sf::Joystick::isConnected(joystickNum(i));
    //}
    for (int i=0; i<joystickCount; i++) {
        bool thisOne = sf::Joystick::isConnected(i);
        result = result || thisOne;
        if (thisOne) {
            setJoystickControls(i);
        }
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
  // cout << "Key pressed " << key << endl;
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
	case '5': playerPause(0,newBool,true); break;
	case '6': playerPause(1,newBool,true); break;
	case '7': playerPause(2,newBool,true); break;
	case '8': playerPause(3,newBool,true); break;
	case '1': playerJoin(0,newBool,true); break;
	case '2': playerJoin(1,newBool,true); break;
	case '3': playerJoin(2,newBool,true); break;
	case '4': playerJoin(3,newBool,true); break;
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
    if (getCubiorPlaying(b) && keyboardControls(b)) {
      upKey[b] = newBool;
    } else {
      setUpCommand(a,newBool);
    }
    break;
	case 'a': case 'A':
    if (getCubiorPlaying(b) && keyboardControls(b)) {
      leftKey[b] = newBool;
    } else {
      setLeftCommand(a,newBool);
    }
    break;
	case 's': case 'S':
    if (getCubiorPlaying(b) && keyboardControls(b)) {
      downKey[b] = newBool;
    } else {
      setDownCommand(a,newBool);
    }
    break;
	case 'd': case 'D':
    if (getCubiorPlaying(b) && keyboardControls(b)) {
      rightKey[b] = newBool;
    } else {
      setRightCommand(a,newBool);
    }
    break;
  case 'f': case 'F':   if (keyboardControls(b)) {  setJump(b,newBool); } break;
  case 'g': case 'G':   if (keyboardControls(b)) {  setLock(b,newBool); } break;
	case 'h': case 'H':   if (keyboardControls(b)) { setSuper(b,newBool); } break;

		// PLAYER 3
	case 'i': case 'I':   if (keyboardControls(c)) {    upKey[c] = newBool; } break;
	case 'u': case 'U':   if (keyboardControls(c)) {  leftKey[c] = newBool; } break;
	case 'o': case 'O':   if (keyboardControls(c)) {  downKey[c] = newBool; } break;
	case 'p': case 'P':   if (keyboardControls(c)) { rightKey[c] = newBool; } break;
  case '[':  case '{':  if (keyboardControls(c)) {    setJump(c,newBool); } break;

		// PLAYER 4
  case 'k': case 'K':   if (keyboardControls(d)) {    upKey[d] = newBool; } break;
  case 'j': case 'J':   if (keyboardControls(d)) {  leftKey[d] = newBool; } break;
  case 'l': case 'L':   if (keyboardControls(d)) {  downKey[d] = newBool; } break;
  case ';': case ':':   if (keyboardControls(d)) { rightKey[d] = newBool; } break;
  case '\'':  case '"': if (keyboardControls(d)) {    setJump(d,newBool); } break;
		/*
		case '1': case '!':
		localPlayerCount = 1;
		break;
		case '2': case '@':
		localPlayerCount = 2;
		break;
		*/
	default: break;
	}
}

// Gives whether some player is using keyboard controls (not joystick)
bool keyboardControls(int player) {
  return keyboard[player];
  return true;
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