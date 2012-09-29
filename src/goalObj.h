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
  protected:
    bool glow;
  public:
    GoalObj();
    void collisionEffect(CubeObj*);
    bool getGlow();
    void setGlow(bool);
};

#endif 
