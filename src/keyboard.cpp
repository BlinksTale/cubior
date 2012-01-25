/*
 * keyboard.cpp
 * by Brian Handy
 * 1/23/12
 * keyboard for cubior
 */

#include <iostream>

using namespace std;

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Handle keyboard input.
void handleInput(unsigned char key, int x, int y) {
  switch(key) {
    case 32: // spacebar
      cout << "hit space!\n";
      break;
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

// nonASCII keys go here
void handleSpecialInput(int key, int x, int y) {
  switch(key) {
    case GLUT_KEY_UP:
      cout << "hit up!\n";
      break;
    case GLUT_KEY_DOWN:
      cout << "hit down!\n";
      break;
    case GLUT_KEY_LEFT:
      cout << "hit left!\n";
      break;
    case GLUT_KEY_RIGHT:
      cout << "hit right!\n";
      break;
    default:
      break;
  }
}
