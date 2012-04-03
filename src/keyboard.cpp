/*
 * keyboard.cpp
 * by Brian Handy
 * 1/23/12
 * keyboard for cubior
 */

#include <iostream>
#include "gameplay.h"

using namespace std;

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

const int playerCount = 4;
const bool jumpingEnabled = true;
const bool lockingEnabled = false;
const bool superEnabled = false;

bool upKey[playerCount];
bool downKey[playerCount];
bool leftKey[playerCount];
bool rightKey[playerCount];
bool jumpKey[playerCount];
bool lockKey[playerCount];
bool superKey[playerCount];
bool pauseKey[playerCount];
int lastPause = -1; // Last player to pause

void setJump(int p, bool b) { if (jumpingEnabled) { if (lockKey[p] && !jumpKey[p] && b) { lockKey[p] = false; } jumpKey[p] = b; } }
void setLock(int p, bool b)  { if (lockingEnabled) { if (jumpKey[p] && !lockKey[p] && b) { jumpKey[p] = false; } lockKey[p] = b; } }
void setSuper(int p, bool b) { if (superEnabled) { superKey[p] = b; } }

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

int getLastPause() { return lastPause; }

// Once per loop, send off the commands from these inputs
void sendCommands() {
  if (getGameplayRunning()) {
    for (int i = 0; i<playerCount; i++) {
      if (    upKey[i]) { getPlayer(i)->moveZ(-10); }
      if (downKey[i]) { getPlayer(i)->moveZ( 10); }
      if (   leftKey[i]) { getPlayer(i)->moveX(-10); }
      if (rightKey[i]) { getPlayer(i)->moveX( 10); }
      getPlayer(i)->jump(jumpKey[i]);
      getPlayer(i)->setLock(lockKey[i]);
      getPlayer(i)->setInvincibility(superKey[i]);
    }
  }
}

// Handle keyboard input.
void handleInput(unsigned char key, bool newBool) {
  switch(key) {
    // SYSTEM CONTROLS
    case '0':  enableGoodCollision(); break;
    case '9': disableGoodCollision(); break;
    case 13 : // Press Enter to pause from p1
    case '5': playerPause(0,newBool); break;
    case '6': playerPause(1,newBool); break;
    case '7': playerPause(2,newBool); break;
    case '8': playerPause(3,newBool); break;
    case '1': setCubiorsPlaying(1); break;
    case '2': setCubiorsPlaying(2); break;
    case '3': setCubiorsPlaying(3); break;
    case '4': setCubiorsPlaying(4); break;
    
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

void handleJoystickInput(int button, bool b) {
  cout << "It was called for " << button << "!\n";
}

void joystickDown(unsigned int button, int x, int y, int z) { handleJoystickInput(button, true); }
