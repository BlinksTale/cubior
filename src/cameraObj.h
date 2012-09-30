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
    bool wallState, intendedState, goalState, freeState;
    static const int camArraySize = 16; // 30 is smooth, 8 is jerky, 16 works.
    int currentCamSlot;
    
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
    int meanX, meanY, meanZ;
    float meanAngleX, meanAngleY, meanAngleZ;
    int camPosX[camArraySize];
    int camPosY[camArraySize];
    int camPosZ[camArraySize];
    float camAngleX[camArraySize];
    float camAngleY[camArraySize];
    float camAngleZ[camArraySize];
    bool lockedToPlayer;
    bool lockedToPlayerX;
    bool lockedToPlayerZ;
    float lockedX;
    float lockedY;
    float lockedZ;
    float lockedAngleY;
    
  public:
    CameraObj();
    void resetPos();
    
    void updateCamArray();
    void updateMeans();
    
    void tick();
    void alwaysFollow(CubeObj*,CubeObj*);
    bool goalWithinJumpRange();
    bool goalWithinDistRange();
    bool goalOutsideDistRange();
    bool goalWithinNearRange();
    bool goalOutsideNearRange();
    float findFollowingBothSide(float, float);
    bool withinRangeOf(int,int,int);
    bool outsideRangeOf(int,int);
    float matchRangeOf(float,float);
    float smoothMatchRangeOf(float,float);
    
    void lookAtPlayer(int,int,int,int,bool,int);
    void lookAtTarget();
    int lookAtBoth(int,int,int);
    void checkExtremeCatchup(int,int,int,int,int);
    
    // functions necessary for follow to work:
    int findIntendedDist(int,int);
    float findTargetY(int,bool);
    
    void follow(int,int,int,int,bool,int);
    float followOne(float,int,int,int);
    float followBoth(float);
    void positionByAngles(int,int,int,int,float,int);
    void maintainPosition();
    
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
    int getMean(int);
    int getMeanX();
    int getMeanY();
    int getMeanZ();
    float getAngle(int);
    float getAngleX();
    float getAngleY();
    float getAngleZ();
    float getRadiansAngleY();
    float getMeanAngle(int);
    float getMeanAngleX();
    float getMeanAngleY();
    float getMeanAngleZ();
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
    
    // Find out if angle and pos locked to that of player
    bool getLockedToPlayer();
    bool getLockedToPlayerX();
    bool getLockedToPlayerZ();
    void resetLocks();
    void setLockedToPlayer(bool);
    void setLockedToPlayerX(bool);
    void setLockedToPlayerZ(bool);
    
    // to get and force camera state info
    int getState();
    void setState(int);
};

#endif 
