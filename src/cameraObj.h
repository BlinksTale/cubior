/*
 * cameraObj.h
 * by Brian Handy
 * 3/29/12
 * Header for any camera in the game
 */

#ifndef CAMERAOBJ
#define CAMERAOBJ

#include "cubeObj.h"

class CameraObj {
  protected:
    int x, y, z;
    CubeObj* permanentTarget;
  public:
    CameraObj();
    void tick();
    void alwaysFollow(CubeObj*);
    void follow(int,int,int);
    int get(int);
    int getX();
    int getY();
    int getZ();
    void setPos(int,int,int);
    void setX(int);
    void setY(int);
    void setZ(int);
};

#endif 
