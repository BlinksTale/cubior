/*
 * cubeShape.h
 * by Brian Handy
 * 2/2/12
 * header for 3d model of cube
 */
#ifndef CUBESHAPE
#define CUBESHAPE

class CubeShape {
  protected:
    bool alternatingSpot;
    float altDark;
    float r1,g1,b1,r2,b2,g2;
  public:
    virtual void initVisuals(float,float,float,float,bool);
    virtual void updateVisuals();
    virtual void draw();
};

#endif
