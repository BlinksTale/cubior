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

int playerCount = 1;

bool upKey = false;
bool downKey = false;
bool leftKey = false;
bool rightKey = false;
bool jumpKey = false;
bool lockKey = false;
bool superKey = false;

bool upKey2 = false;
bool downKey2 = false;
bool leftKey2 = false;
bool rightKey2 = false;
bool jumpKey2 = false;
bool lockKey2 = false;
bool superKey2 = false;

// Once per loop, send off the commands from these inputs
void sendCommands() {
  if (upKey)    { getPlayer(0)->moveZ(-10); }
  if (downKey)  { getPlayer(0)->moveZ( 10); }
  if (leftKey)  { getPlayer(0)->moveX(-10); }
  if (rightKey) { getPlayer(0)->moveX( 10); }
  getPlayer(0)->jump(jumpKey);
  getPlayer(0)->setLock(lockKey);
  getPlayer(0)->setInvincibility(superKey);

  if (upKey2)    { getPlayer(1)->moveZ(-10); }
  if (downKey2)  { getPlayer(1)->moveZ( 10); }
  if (leftKey2)  { getPlayer(1)->moveX(-10); }
  if (rightKey2) { getPlayer(1)->moveX( 10); }
  getPlayer(1)->jump(jumpKey2);
  getPlayer(1)->setLock(lockKey2);
  getPlayer(1)->setInvincibility(superKey2);
}

// Handle keyboard input.
void handleInput(unsigned char key, bool newBool) {
  switch(key) {
    case 32: // spacebar
      break;
// OLD PLAYER 1 CONTROLS (although this If statement doesn't seem to work)
if (playerCount == 1) {
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
} else {
    // NEW PLAYER 1
      case 'm': case 'M':
      superKey = newBool;
      break;
    case ',': case '<':
      if (jumpKey && !lockKey && newBool) { jumpKey = false; }
      lockKey = newBool;
      break;
    case '.': case '>':
      if (lockKey && !jumpKey && newBool) { lockKey = false; }
      jumpKey = newBool;
      break;

    // PLAYER 2
    case 'w': case 'W':
      upKey2 = newBool;
      break;
    case 'a': case 'A':
      leftKey2 = newBool;
      break;
    case 's': case 'S':
      downKey2 = newBool;
      break;
    case 'd': case 'D':
      rightKey2 = newBool;
      break;
    case 'f': case 'F':
      if (lockKey2 && !jumpKey2 && newBool) { lockKey2 = false; }
      jumpKey2 = newBool;
      break;
    case 'g': case 'G':
      if (jumpKey2 && !lockKey2 && newBool) { jumpKey2 = false; }
      lockKey2 = newBool;
      break;
    case 'h': case 'H':
      superKey2 = newBool;
      break;
}

    case '1': case '!':
      playerCount = 1;
      break;
    case '2': case '@':
      playerCount = 2;
      break;

    // QUIT
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
