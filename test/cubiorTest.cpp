/*
 * cubiorTest.cpp
 * by Brian Handy
 * 2/11/12
 * Test class for 3d platformer
 */
#include <cmath>
#include <iostream>
#include <string>
#include <string.h>
#include "../src/gameplay.h"
#include "../src/cubiorObj.h"
#include "../src/cubeObj.h"
using namespace std;

string truth(bool n) { return n ? "true" : "false"; }

void setPos(CubeObj* c1, int a, int b, int c) {
  cout << "Set CubeObj to " << a << "," << b << "," << c << "\n";
  c1->setX(a);
  c1->setY(b);
  c1->setZ(c);
}

void chkPos(CubeObj* c1, int a, int b, int c) {
  cout << "\ngetX() == " << a << " returned " + truth(a == c1->getX());
  cout << "\ngetY() == " << b << " returned " + truth(b == c1->getY());
  cout << "\ngetZ() == " << c << " returned " + truth(c == c1->getZ());
  cout << "\n\n";
}

void prtPos(CubeObj* c1) {
  cout << "x: " << c1->getX() << ", y: " << c1->getY() << ", z: " << c1->getZ() << "\n";
}

int main(int argc, char** argv) {
  CubiorObj cubior;
  int cubeCount = 6;
  int cubeBreak = 3;
  CubeObj cube[cubeCount];
  cout << "\n---Cubior Unit Test---\n\n";

  setPos(&cubior,110,0,-100);
  chkPos(&cubior,110,0,-100);

  for (int i = 0; i<cubeCount; i++){
    cube[i].setLock(true);
    cout << "cube locked = " << truth(cube[i].getLock()) << "\n";

    setPos(&cube[i],(i%cubeBreak)*100,0-100*(i/cubeBreak),0-100*(i/cubeBreak));
    chkPos(&cube[i],(i%cubeBreak)*100,0-100*(i/cubeBreak),0-100*(i/cubeBreak));
  }

  prtPos(&cubior);
  cout << "\n";  

    //cubior.tick();
    //cubior.tick();
  // Try falling and hitting the cube
  for (int i=0; i<7; i++) {
    cubior.tick();
    cubior.moveX(-10);
    cubior.moveZ( 10);
    cout << "Move:  ";
    prtPos(&cubior);
  
    for (int j=0; j<cubeCount; j++) {
    bool collided = collision(&cubior, &cube[j]);
    cout << "They hit? " << truth(collided) << "\n";
    if (collided) {
      bounce(&cubior, &cube[j]);
      cout << "Bounce: ";
      prtPos(&cubior);
    }
    cout << "\n";
    }
  }

  cout << "---All tests returned---\n\n";
  return 0;
}


