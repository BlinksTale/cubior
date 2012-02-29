/*
 * cubiorTest.cpp
 * by Brian Handy
 * 2/11/12
 * Test class for 3d platformer
 */
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
  CubeObj cube;
  cout << "\n---Cubior Unit Test---\n\n";

  cube.setLock(true);
  cout << "cube locked = " << truth(cube.getLock()) << "\n";
  cout << "cubior locked = " << truth(cubior.getLock()) << "\n";

  setPos(&cubior,0,100,0);
  chkPos(&cubior,0,100,0);

  setPos(&cube,0,0,0);
  chkPos(&cube,0,0,0);

  prtPos(&cubior);
  cout << "\n";  

    cubior.tick();
    cubior.tick();
  // Try falling and hitting the cube
  for (int i=0; i<3; i++) {
  
    cout << "Tick:  ";
    prtPos(&cubior);
  
    bool collided = collision(&cubior, &cube);
    cout << "They hit? " << truth(collided) << "\n";
    if (collided) {
      bounce(&cubior, &cube);
      cout << "Bounce: ";
      prtPos(&cubior);
    }
    cout << "\n";
  }

  cout << "---All tests returned---\n\n";
  return 0;
}


