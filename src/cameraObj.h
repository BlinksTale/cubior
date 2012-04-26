/*
 * cameraObj.h
 * by Brian Handy
 * 3/29/12
 * Header for any camera in the game
 */

#ifndef CAMERAOBJ
#define CAMERAOBJ

#include "cubeObj.h"
#include "trackerObj.h"

class CameraObj {
  protected:
    static const int camHeight = 600;
    int x, y, z, farthestDist, closestDist, idealDist, lastLandedY;
    bool lastLanded;
    float angleX, angleY, angleZ;
    CubeObj* permanentTarget;
    TrackerObj* tracker;
  public:
    CameraObj();
    void tick();
    void alwaysFollow(CubeObj*);
    bool withinRangeOf(int,int,int);
    int matchRangeOf(int,int);
    void follow(int,int,int,int,bool,int);

    int get(int);
    int getX();
    int getY();
    int getZ();
    float getAngle(int);
    float getAngleX();
    float getAngleY();
    float getAngleZ();
    void setPos(int,int,int);
    void setX(int);
    void setY(int);
    void setZ(int);
    void setAngleX(float);
    void setAngleY(float);
    void setAngleZ(float);
    void setAngle(float,float,float);
};

#endif 
