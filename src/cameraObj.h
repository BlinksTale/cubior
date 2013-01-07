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
    static const bool showData = false;
    bool droppingIn; // used to start a level, won't try option five until false
    bool locksReset; // make sure you don't lock before the locks are reset
    bool wallState, intendedState, goalState, freeState;
    bool haventPlayedFailSfx; // stops fail sfx from playing repeatedly
    bool playerCommandsLastTime; // any commands from player last tick?
    // Would love to have this higher/smoother, but doesn't keep up with player
    // when following along a wall angle.
    static const int camArraySize = 16; // 30 is smooth, 8 is jerky, 16 works.
    int currentCamSlot;

    // For cam controls
    bool playerCenterCommand, playerLeftCommand, playerRightCommand, playerUpCommand, playerDownCommand;
    bool playerCommandActive; // whether still adhering to player set angle or not
    int playerCommandAngle; // so that we keep player's angle until we move outside it naturally
    int playerCommandHeight; // same goes for commanded height
    static const int playerCommandMargin = 60; // how far we can turn before leaving player set angle

    static const int camHeight = 600, goalRange = 800, camSpeed = 30, intendedStuckMax = 2;
    int camCommandedHeight;
    int farthestDist, closestDist, idealDist, lastLandedY, cameraSide,
           visibleIntendedCount, lastDistToIntended, intendedStuckCount;
    bool lastLanded, followingBoth, nearGoal, los, backupFreedom;

    // bools for checkCommandLock history
    bool oldFollowingBoth, oldXNear, oldXFar, oldZNear, oldZFar;
    bool tryXNear, tryXFar, tryZNear, tryZFar;

    float angleX, angleY, angleZ;
    CubeObj* permanentTarget;
    CubeObj* permanentTargetGoal;
    CubeObj intendedPos;
    bool foundIntendedPos;
    bool freedom;
    TrackerObj* tracker;
    int permanentTargetNum;
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
    float lastAngleY;
    float lastAngleDelta;
    bool justFixedVisibility;
    int justFixedX;
    int justFixedZ;
    static const int justFixedMaxDist = 100;
    
    // Camera commands
    bool hasCommandedAngle;
    int commandedAngleY;
    int commandedHeight;
    int commandedHyp;

  public:
    CameraObj();
    void resetPos();
    
    void resetCamArray();
    void updateCamArray();
    void updateMeans();
    
    void applyJustFixedVisibility(); // first time setup when establishing that we just fixed it
    void updateJustFixedVisibility(); // ensure we do start moving again if needed
    void checkCommandLock(); // freedom from player command if new type of angle request 
    void restoreCameraFreedom(); // return freedom to camera after player control is done
    bool freeMovementState(); // returns if Option 1 or not
    void applyFreeMovement();
    void applyLockedToPlayer();
    void applyLockedToPlayerX();
    void applyLockedToPlayerZ();
    void applyLockedToTracker(bool); // used for all locked to player vals
    void applyIntendedPos();

    // Player Camera Controls
    bool matchAngleY(int);
    bool matchHeight(int);
    int getMatchingPos(int,int);
    void applyMatchHeight(int,bool);
    void applyMatchAngleY(int,bool);
    void applyCommandAngle();
    void checkCommandAngle();
    bool getPlayerCommandActive();

    bool getSnapLock();
    void snapLock();
    float getLockedAngleY();

    void tick();
    void alwaysFollow(CubeObj*,CubeObj*,int);
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
    float fixedDeltasToDegrees(int, int);

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
    
    // For making sure gameplay's wall angle finder
    // doesn't screw up what we just fixed
    bool getJustFixedVisibility() { return justFixedVisibility; }
    void setJustFixedVisibility(bool b) { justFixedVisibility = b; }
    
    // to get and force camera state info
    int getState();
    void setState(int);
    bool getDroppingIn(); // to check if just starting out

    // Collision handling
    void hitSomething();

    // Set camera commands
    void setPlayerCommandActive(bool);
    void setPlayerCenterCommand(bool);
    void setPlayerLeftCommand(bool);
    void setPlayerRightCommand(bool);
    void setPlayerUpCommand(bool);
    void setPlayerDownCommand(bool);

    void tryingXNear(bool);
    void tryingXFar(bool);
    void tryingZNear(bool);
    void tryingZFar(bool);
};

#endif 
