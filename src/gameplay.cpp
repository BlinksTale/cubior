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
int angleZ = 0;
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

// Sets gameplay state
void setPlayerX(int n) { cubior.setX(n); } 
void setPlayerY(int n) { cubior.setY(n); } 
void setPlayerZ(int n) { cubior.setZ(n); } 
void setPlayerAngleZ(int newAngleZ) { angleZ = newAngleZ; }
void changePlayerX(int n) { cubior.changeX(n); } 
void changePlayerY(int n) { cubior.changeY(n); } 
void changePlayerZ(int n) { cubior.changeZ(n); } 
void changePlayerAngleZ(int newAngleZ) { angleZ += newAngleZ; }
void movePlayerX(int n) { cubior.moveX(n); }
void movePlayerY(int n) { cubior.moveY(n); }
void movePlayerZ(int n) { cubior.moveZ(n); }
void movePlayerAngleZ(int newAngleZ) { angleZ += newAngleZ * rotationSpeed; }
void jump(bool newJump) { cubior.jump(newJump); }

// Returns gameplay state
CubeObj* getPlayer() { return &cubior; }
int getPlayerX() { return cubior.getX(); }
int getPlayerY() { return cubior.getY(); }
int getPlayerZ() { return cubior.getZ(); }
int getPlayerAngleZ() { return angleZ; }

bool getLocking() { return cubior.getLock(); }
void setLocking(bool n) {
  cubior.setLock(n);
}
bool getInvincibility() { return invincible; }
void setInvincibility(bool newState) { invincible = newState; }
float getHappiness() { return happiness; }

int getFloor() { return floor; }
int getGravity() { return gravity; }
