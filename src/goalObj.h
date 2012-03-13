/*
 * goalObj.h
 * by Brian Handy
 * 3/7/12
 * Header for the goal of each level
 */

#ifndef GOALOBJ
#define GOALOBJ

#include "cubeObj.h"

class GoalObj : public CubeObj {
  public:
    GoalObj();
    void collisionEffect(CubeObj*);
};

#endif 
