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
  public:
    void initCubeVisuals();
    void updateCubeVisuals();
    void drawCube(float,float,float,float);
};

#endif
