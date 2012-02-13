/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */
#include "gameplay.h"
#include "cubeObj.h"
#include "cubiorObj.h"

CubiorObj cubior[4];
static int movementSpeed = 1;
static int jumpSpeedRatio = 5;
static int rotationSpeed = 10;
int maxJump = 25;
int maxSpeed = 20;
int friction = 1;

int gravity = 2;
int floor = -100;

void gameplayStart() {
  cubior[0].setPos(0,0,-1000);
  cubior[0].moveZ(3);
  cubior[1].setPos(-400,0,-1000);
  cubior[0].setHappiness(1.0);
  cubior[1].setHappiness(0.5);
}

void gameplayLoop() {
  cubior[0].tick();
  cubior[1].tick();
}

// Returns gameplay state
CubiorObj* getPlayer() { return &cubior[0]; }
CubiorObj* getPlayer(int i) { return &cubior[i]; }

bool getInvincibility(int n) { return cubior[n].getInvincibility(); }
void setInvincibility(int n, bool newState) { cubior[n].setInvincibility(newState); }

int getFloor() { return floor; }
int getGravity() { return gravity; }
