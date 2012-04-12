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
    int x, y, z, angleX, angleY, angleZ, farthestDist;
    CubeObj* permanentTarget;
  public:
    CameraObj();
    void tick();
    void alwaysFollow(CubeObj*);
    void follow(int,int,int,bool);
    int get(int);
    int getX();
    int getY();
    int getZ();
    int getAngle(int);
    int getAngleX();
    int getAngleY();
    int getAngleZ();
    void setPos(int,int,int);
    void setX(int);
    void setY(int);
    void setZ(int);
    void setAngleX(int);
    void setAngleY(int);
    void setAngleZ(int);
    void setAngle(int,int,int);
};

#endif 
