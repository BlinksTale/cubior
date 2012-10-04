/*
 * Cubior
 * by Brian Handy
 * 1/13/12
 * Main class for 3d platformer
 */
#include <iostream>
#include "visuals.h"
#include "gameplay.h"
using namespace std;

// Gets called as often as possible
void tick() {
  //gameplayLoop();
  //updateGraphics(x);
  //render();
}

int main(int argc, char** argv) {
  gameplayStart("./maps/cubiorMap0.cubior");
  initRender(argc, argv); // nothing gets called after this
  tick();
  return 0;
}

