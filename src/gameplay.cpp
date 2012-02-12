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
  y = cubior.getY();//floor;
  z = cubior.getZ();
}

void gameplayLoop() {
  cubior.tick();
/*
  // demo animation: angleZ += rotationSpeed / 10;
  // base smile on rotation
  //happiness = (angleZ % 360 - 120) / 360.0 * 2.0;
  // don't move if frozen
  if (!locked) {
    // cap momentum on ground
    if (momentumX > maxSpeed) { momentumX = maxSpeed; }
    if (momentumX < -maxSpeed) { momentumX = -maxSpeed; }
    if (momentumZ > maxSpeed) { momentumZ = maxSpeed; }
    if (momentumZ < -maxSpeed) { momentumZ = -maxSpeed; }
    // Then move by that momentum
    x += momentumX; 
    y += momentumY;
    z += momentumZ; 

    // apply friction if on the ground
    if (grounded) {
      if (momentumX > 0) { momentumX -= friction; }
      else if (momentumX < 0) { momentumX += friction; }
      else { momentumX = 0; }
      if (momentumZ > 0) { momentumZ -= friction; }
      else if (momentumZ < 0) { momentumZ += friction; }
      else { momentumZ = 0; }
    }    
  } else if (playerMoving() && lockingLosesMomentum) { freezePlayer(); }
  if (!locked || lockingLosesMomentum) { fall(); }
*/
}

// Apply gravity! Stop if you hit the floor
void fall() {
  //if (y < floor) {
  //  y = floor; momentumY = 0; lockable = true; jumpable = true; grounded = true;
  //} else { momentumY -= gravity; grounded = false; } 
}

bool playerMoving() {
 //return (momentumX != 0 || momentumZ != 0 || momentumY != 0);
}
// Stop all movement for player!
void freezePlayer() {
   // momentumX = 0;
   // momentumY = 0;
   // momentumZ = 0;
}
// Sets gameplay state
void setPlayerX(int n) { cubior.setX(n); } //x = newX; }
void setPlayerY(int n) { cubior.setY(n); } //y = newY; }
void setPlayerZ(int n) { cubior.setZ(n); } //z = newZ; }
void setPlayerAngleZ(int newAngleZ) { angleZ = newAngleZ; }
void changePlayerX(int n) { cubior.changeX(n); } //x += n; }
void changePlayerY(int n) { cubior.changeY(n); } //y += n; }
void changePlayerZ(int n) { cubior.changeZ(n); } //z += n; }
void changePlayerAngleZ(int newAngleZ) { angleZ += newAngleZ; }
void movePlayerX(int n) { cubior.moveX(n); } //momentumX += n * movementSpeed; }
void movePlayerY(int n) { cubior.moveY(n); } //momentumY += n * movementSpeed; }
void movePlayerZ(int n) { cubior.moveZ(n); } //momentumZ += n * movementSpeed; }
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
//  if ((lockable && newLock) || !newLock) { locked = newLock; }
  // Disabling this right now, cannot jump from a lock
  //if (locked) { lockable = false; jumpable = true; }
}
bool getInvincibility() { return invincible; }
void setInvincibility(bool newState) { invincible = newState; }
float getHappiness() { return happiness; }

int getFloor() { return floor; }
int getGravity() { return gravity; }
