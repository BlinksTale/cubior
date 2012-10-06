/*
 * cubiorShape.h
 * by Brian Handy
 * 2/12/12
 * header for 3d model of cube
 */

#include "cubeShape.h"

#ifndef CUBIORSHAPE
#define CUBIORSHAPE

class CubiorShape : public CubeShape {
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
    void initCubiorVisuals(int);
    void updateCubiorVisuals(int);
    void updateCubiorColors(int);
    void drawCubior(int);
    void drawCubiorSilhouette();
    void drawEyeAt(float,float,float);
    void drawClosedEyeAt(float,float,float,bool);
};
#endif
