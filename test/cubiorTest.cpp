/*
 * cubiorTest.cpp
 * by Brian Handy
 * 2/11/12
 * Test class for 3d platformer
 */
#include <iostream>
#include <string>
#include <string.h>
#include "../src/cubeObj.h"
using namespace std;

string truth(bool n) { return n ? "true" : "false"; }

int main(int argc, char** argv) {
  CubeObj cubior;
  cout << "\nCubior Unit Test\n";
  
  cout << "Testing cubeObj getX\n";
  cubior.setX(0);
  cout << "\ngetX() == 0 returned " + truth(0 == cubior.getX());
  cubior.setX(1);
  cout << "\ngetX() == 1 returned " + truth(1 == cubior.getX());
  cubior.setX(2);
  cout << "\ngetX() == 2 returned " + truth(2 == cubior.getX());

  cout << "\n\nAll tests returned\n";
  return 0;
}


