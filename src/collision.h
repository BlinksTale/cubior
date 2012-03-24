/*
 * collision.h
 * by Brian Handy
 * 2/29/12
 * Header for all collision checks
 */

#ifndef COLLISION
#define COLLISION

#include "cubeObj.h"

class Collision {
  protected:
    int totalCollisions;
  public:
    Collision();

    static bool between(CubeObj*,CubeObj*);
    static int  getDiff(CubeObj*,CubeObj*,int);
    static void bounce(CubeObj*,CubeObj*);
    static void bounceByDiff(CubeObj*,CubeObj*,int,int,int);
    static void bouncePrecisely(CubeObj*,CubeObj*);
    static void balanceMomentum(CubeObj*,CubeObj*,int);

    static void checkAndBounce(CubeObj*,CubeObj*);
};

#endif 
