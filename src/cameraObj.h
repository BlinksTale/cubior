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
    static const int camHeight = 600, goalRange = 800, camSpeed = 30, intendedStuckMax = 2;
    int farthestDist, closestDist, idealDist, lastLandedY, cameraSide,
           visibleIntendedCount, lastDistToIntended, intendedStuckCount;
    bool lastLanded, followingBoth, nearGoal, los, backupFreedom;
    float angleX, angleY, angleZ;
    CubeObj* permanentTarget;
    CubeObj* permanentTargetGoal;
    CubeObj intendedPos;
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
    float findTargetY(int,bool);
    
    void lookAtPlayer(int,int,int,int,bool,int);
    void follow(int,int,int,int,bool,int);
    float followOne(float,int,int,int);
    float followBoth(float);
    void positionByAngles(int,int,int,int,float,int);
    
    int getFarthestDist();
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
    bool getFoundIntendedPos();
    void setFoundIntendedPos(bool);
    void disableIntendedPos();
    // to make sure it doesn't go too long towards that location
    // when the player is, in fact, already visible.
    int getVisibleIntended();
    void setVisibleIntended(int);
    
    // Along intended pos is the freedom to backup... or lack thereof
    void setBackupFreedom(bool);
    bool getBackupFreedom();
    
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
