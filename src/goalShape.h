/*
 * goalShape.h
 * by Brian Handy
 * 3/7/12
 * header for 3d model of goal
 */

#include "cubeShape.h"

#ifndef GOALSHAPE
#define GOALSHAPE

class GoalShape : public CubeShape {
  protected:
    float r1,g1,b1,r2,g2,b2;

    float colorDarkness;
    float colorDefaultA;
    float colorDefaultR;
    float colorDefaultG;
    float colorDefaultB;
    float colorCurrentR;
    float colorCurrentG;
    float colorCurrentB;

  public:
    void initVisuals();
    void updateVisuals();
    void updateColors();
    void drawGoal();
};
#endif
