/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */
#include "gameplay.h"

int x = -9;
int y = -9;
int momentumX = 0;
int momentumY = 0;
int angleZ = 0;
static int movementSpeed = 2;
static int rotationSpeed = 10;
float happiness = 1.0;
bool locked = false;
bool lockable = false;
bool jumpable = false;
bool grounded = false;
int maxJump = 20;
int maxSpeed = 20;
int friction = 1;

int gravity = 1;
int floor = -100;

void gameplayLoop() {
  // demo animation: angleZ += rotationSpeed / 10;
  // base smile on rotation
  //happiness = (angleZ % 360 - 120) / 360.0 * 2.0;
  // don't move if frozen
  if (!locked) {
    // cap momentum on ground
    if (momentumX > maxSpeed) { momentumX = maxSpeed; }
    if (momentumX < -maxSpeed) { momentumX = -maxSpeed; }
    // Then move by that momentum
    x += momentumX; 
    y += momentumY;

    // apply friction if on the ground
    if (grounded) {
      if (momentumX > 0) { momentumX -= friction; }
      else if (momentumX < 0) { momentumX += friction; }
      else { momentumX = 0; }
    }    
  } else if (momentumX != 0 || momentumY != 0) {
    momentumX = 0;
    momentumY = 0;
  }
  if (y < floor) {
    y = floor; momentumY = 0; lockable = true; jumpable = true; grounded = true;
  } else { momentumY -= gravity; grounded = false; } 
}

// Sets gameplay state
void setPlayerX(int newX) { x = newX; }
void setPlayerY(int newY) { y = newY; }
void setPlayerAngleZ(int newAngleZ) { angleZ = newAngleZ; }
void changePlayerX(int newX) { x += newX; }
void changePlayerY(int newY) { y += newY; }
void changePlayerAngleZ(int newAngleZ) { angleZ += newAngleZ; }
void movePlayerX(int newX) { momentumX += newX * movementSpeed; }
void movePlayerY(int newY) { momentumY += newY * movementSpeed; }
void movePlayerAngleZ(int newAngleZ) { angleZ += newAngleZ * rotationSpeed; }
void jump(bool newJump) { 
  if (jumpable) {
    if (newJump && momentumY < maxJump) { movePlayerY(2); } else { jumpable = false; }
  }
}

// Returns gameplay state
int getPlayerX() { return x; }
int getPlayerY() { return y; }
int getPlayerAngleZ() { return angleZ; }

bool getLocking() { return locked; }
void setLocking(bool newLock) {
  if ((lockable && newLock) || !newLock) { locked = newLock; }
  if (locked) { lockable = false; jumpable = true; }
}
float getHappiness() { return happiness; }
