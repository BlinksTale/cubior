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

class CameraObj : public CubeObj {
// FIXME: extending CubeObj may cause lag, but not sure. Check later
  protected:
    static const int camHeight = 600, goalRange = 800, camSpeed = 1;
    int x, y, z, farthestDist, closestDist, idealDist, lastLandedY, cameraSide;
    bool lastLanded, followingBoth, nearGoal, los;
    float angleX, angleY, angleZ;
    CubeObj* permanentTarget;
    CubeObj* permanentTargetGoal;
    CubeObj* intendedPos;
    bool foundIntendedPos;
    bool freedom;
    TrackerObj* tracker;
  public:
    CameraObj();
    void resetPos();
    void tick();
    void alwaysFollow(CubeObj*,CubeObj*);
    bool goalWithinJumpRange();
    bool goalWithinDistRange();
    bool goalOutsideDistRange();
    bool goalWithinNearRange();
    bool goalOutsideNearRange();
    float findFollowingBothSide(float, float);
    bool withinRangeOf(int,int,int);
    float matchRangeOf(float,float);
    float smoothMatchRangeOf(float,float);
    
    int lookAtBoth(int,int,int);
    void checkExtremeCatchup(int,int,int,int,int);
    
    // functions necessary for follow to work:
    int findIntendedDist(int,int);
    float findNewY(int,bool);
    
    void follow(int,int,int,int,bool,int);
    int distToGoal();
    int distToPlayer();
    int heightToPlayer();
    int groundDistToPlayer();
    int groundDistTo(int,int);
    void betweenPlayerAndGoal();
    float angleToGoal();
    float angleBetweenPlayerAndGoal();
    float deltasToDegrees(int, int);

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

    // To move camera to a new, specific location
    void setIntendedPos(CubeObj*);
    void setFoundIntendedPos(bool);
    void disableIntendedPos();
    
    // Relates to cameras ability to recenter and move freely
    // is false after putting player back into visibility
    void setFreedom(bool);
    bool getFreedom();
    
    void setLOS(bool);
    bool getLOS();
    bool getVisibility();
    CubeObj* getPermanentTarget();
};

#endif 
