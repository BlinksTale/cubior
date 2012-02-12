/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */
#include "gameplay.h"
#include "cubeObj.h"

CubeObj cubior;
int x = -0; // was -9
int y = -0;
int z = -0;
int momentumX = 0;
int momentumY = 0;
int momentumZ = 0;
static int movementSpeed = 1;
static int jumpSpeedRatio = 5;
static int rotationSpeed = 10;
float happiness = 1.0;
bool locked = false;
bool lockable = false;
bool lockingLosesMomentum = false;
bool jumpable = false;
bool grounded = false;
bool invincible = false;
int maxJump = 25;
int maxSpeed = 20;
int friction = 1;

int gravity = 2;
int floor = -100;

void gameplayStart() {
  cubior.setPos(0,0,-1000);
  x = cubior.getX();
  y = cubior.getY();
  z = cubior.getZ();
}

void gameplayLoop() {
  cubior.tick();
}

// Returns gameplay state
CubeObj* getPlayer() { return &cubior; }

bool getInvincibility() { return invincible; }
void setInvincibility(bool newState) { invincible = newState; }
float getHappiness() { return happiness; }

int getFloor() { return floor; }
int getGravity() { return gravity; }
