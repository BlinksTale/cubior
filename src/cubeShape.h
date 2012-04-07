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
  public:
    virtual void initVisuals(bool);
    virtual void updateVisuals();
    virtual void draw(float,float,float,float);
};

#endif
