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
    bool glow, lastGlow;
    int glowCount;
    static const int glowMax = 10;
  public:
    GoalObj();
    void collisionEffect(CubeObj*);
    bool getGlow();
    bool getLastGlow();
    bool getGlowMax();
    void setGlow(bool);
};

#endif 
