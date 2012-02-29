/*
 * Gameplay for Cubior
 * by Brian Handy
 * 1/23/12
 * Gameplay class for 3d platformer
 */

#include "gameplay.h"
#include "cubeObj.h"
#include "cubiorObj.h"
#include "collision.h"

using namespace std;

const int cubiorCount = 3;
const int cubeCount = 5;

CubiorObj cubior[cubiorCount];
CubeObj cube[cubeCount];
static int movementSpeed = 1;
static int jumpSpeedRatio = 5;
static int rotationSpeed = 10;
int maxJump = 25;
int maxSpeed = 20;
int friction = 1;

int gravity = 2;
int floor = -100;

void gameplayStart() {
  // Cubior Start States!
  for (int i=0; i<cubiorCount; i++) {
    cubior[i].setPos(-200*i+0,0,-1000);
    cubior[i].moveX(3);
    cubior[i].moveY(3);
    cubior[i].moveZ(3);
    cubior[i].setHappiness(1.0-i*1.0/cubiorCount);
  }
  // and Cube Obstacle start states
  for (int i=0; i<cubeCount; i++) {
    cube[i].setPos(-100*i+0,-100,-1000);
    cube[i].setPermalock(true);
  }
}

void gameplayLoop() {
  // Run main tick loop for all Cubiors and Cubes
  for (int i = 0; i<cubiorCount; i++) {
    cubior[i].tick();
  }
  for (int i = 0; i<cubeCount; i++) {
    cube[i].tick();
  }
  for (int i = 0; i<cubiorCount; i++) {
    // Check collision with other Cubiors
    for (int j = i+1; j<cubiorCount; j++) {
      if (Collision::between(&cubior[i],&cubior[j])) {
        Collision::bounce(&cubior[i],&cubior[j]);
        //Collision::balanceMomentum(&cubior[i],&cubior[j]);
      }
    }
    // Check collision with Obstacles
    for (int j = 0; j<cubeCount; j++) {
      if (Collision::between(&cubior[i],&cube[j])) {
        Collision::bounce(&cubior[i],&cube[j]);
        //Collision::balanceMomentum(&cubior[i],&cube[0]);
      }
    }
  }
}

// Returns gameplay state
CubiorObj* getPlayer() { return &cubior[0]; }
CubiorObj* getPlayer(int i) { return &cubior[i]; }
CubeObj* getCube() { return &cube[0]; }
CubeObj* getCube(int i) { return &cube[i]; }
int getCubiorCount() { return cubiorCount; }
int getCubeCount() { return cubeCount; }

bool getInvincibility(int n) { return cubior[n].getInvincibility(); }
void setInvincibility(int n, bool newState) { cubior[n].setInvincibility(newState); }

int getFloor() { return floor; }
int getGravity() { return gravity; }
