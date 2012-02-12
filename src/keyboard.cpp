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

bool upKey = false;
bool downKey = false;
bool leftKey = false;
bool rightKey = false;
bool jumpKey = false;
bool lockKey = false;
bool superKey = false;

// Once per loop, send off the commands from these inputs
void sendCommands() {
  if (upKey)    { getPlayer()->moveZ(-1); }
  if (downKey)  { getPlayer()->moveZ( 1); }
  if (leftKey)  { getPlayer()->moveX(-1); }
  if (rightKey) { getPlayer()->moveX( 1); }
  getPlayer()->jump(jumpKey);
  getPlayer()->setLock(lockKey);
  setInvincibility(superKey);
}

// Handle keyboard input.
void handleInput(unsigned char key, bool newBool) {
  switch(key) {
    case 32: // spacebar
      break;
    case 'c': case 'C':
      superKey = newBool;
      break;
    case 'x': case 'X':
      if (jumpKey && !lockKey && newBool) { jumpKey = false; }
      lockKey = newBool;
      break;
    case 'z': case 'Z':
      if (lockKey && !jumpKey && newBool) { lockKey = false; }
      jumpKey = newBool;
      break;
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

void inputDown(unsigned char key, int x, int y) { handleInput(key, true); }
void inputUp(unsigned char key, int x, int y) { handleInput(key, false); }

// nonASCII keys go here
void handleSpecialInput(int key, bool newBool) {
  switch(key) {
    case GLUT_KEY_UP:
      upKey = newBool;
      break;
    case GLUT_KEY_DOWN:
      downKey = newBool;
      break;
    case GLUT_KEY_LEFT:
      leftKey = newBool;
      break;
    case GLUT_KEY_RIGHT:
      rightKey = newBool;
      break;
    default:
      break;
  }
}

void specialInputDown(int key, int x, int y) { handleSpecialInput(key, true); }
void specialInputUp(int key, int x, int y)   { handleSpecialInput(key, false); }
