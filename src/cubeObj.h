/*
 * cubeObj.h
 * by Brian Handy
 * 2/11/12
 * Header for any cube in the game (obstacles, items and players)
 */

#ifndef CUBEOBJ
#define CUBEOBJ

//#include "cameraObj.h"
#include <vector>
#include <string>
using namespace std;

class CubeObj {
  protected:
    static const int altSize = 400; // how wide the checker patterns are
    static const int maxMovement = 49; // less than half of a cube width
    static const int maxFall = 49; // less than half of a cube width
    
    string itemType;
    
    bool fpsRateEnabled, duplicateNeighbor;
    bool neighbors[6],visibleNeighbors[6],edges[6],toldToMove,lastToldToMove;
    CubeObj* visibleNeighborObjects[6];
    bool justHitPlayer = false;
    float momentumX, momentumY, momentumZ, movementSpeed, movementDivision, posAverageBias;
    bool toldToMoveX, toldToMoveY, toldToMoveZ;
    float toldToMoveXDist, toldToMoveYDist, toldToMoveZDist; // for immediate directions/instructions
    int x, y, z, diffX, diffY, diffZ, oldX, oldY, oldZ,
        landedOnX, landedOnY, landedOnZ, landedOnCount;
    bool hasMaterial, playerStatus, landableStatus, cameraStatus, newJump, loseMomentumOnLock;
    bool locked, lockable, permalocked, lockedX, lockedY, lockedZ,
        jumpable, grounded, lastGrounded, doubleLastGrounded, neighborsSet, visibleNeighborsSet;
    int maxSpeed, friction, falling, minFalling;
    float maxJump, jumpSpeed, jumpSpeedRatio, gravity;
    int material, justLandedInt, lastMoved;
    bool jumping, lastJumping, collision, lastCollision, justLandedBool;
    bool lastDirectionConflict, lastDirectionFlip;
    CubeObj* landedOn;
    bool landedOnOnline;
    //CameraObj* camera; // for camera cubes to ID their cameras
    float landedOnDirectionDiff, landedOnToldDirectionDiff;
    float newFriction, strength, direction, toldDirection; // for use with new friction technique
    bool invisible, item, solid;
  public:
    float timeSinceCollision, timeSinceCollisionMax, timeColliding, timeCollidingMax, timeSinceFirstCollision, timeSinceFirstCollisionMax;
    bool firstCollision, playerAbove, usingPlayerAbove, nonstickSurface;

    CubeObj();
    virtual void postNeighborInit();
  
    string getType();
    
    virtual void tick();

    void calculateDiff();
    void fall();
    void land();
    void landOn(CubeObj*);
    void resetLandedOn();
    bool updateLandedOnPos();
    float getLandedOnDirectionDiff();
    void setLandedOnDirectionDiff(float);
    int getLandedOnX();
    int getLandedOnY();
    int getLandedOnZ();
    void setLandedOnX(int);
    void setLandedOnY(int);
    void setLandedOnZ(int);
    void setLandedOnPos(int,int,int);
    void setLandedOnPosAverage(int,int,int);
    int getLandedOnCount();
    void freeze();
    
    // return status
    bool isMoving();
    bool isMovingQuickly();
    bool isPlayer();
    bool isLandable();
    bool isCamera();
    void setCameraStatus(bool);
    bool isSolid();
    void setSolid(bool);
    //void setCamera(CameraObj*);
    //CameraObj* getCamera();

    void jump(bool);

    virtual void collisionEffect(CubeObj*);

    void setLock(bool);
    bool getLock();
    bool getLockX();
    bool getLockY();
    bool getLockZ();
    void setPermalock(bool);
    bool getPermalock();
    bool getGrounded();
    bool getStillGrounded();
    bool getStillJumping();
    bool getNotGrounded();
    bool getLanded();
    bool justJumped();
    bool justBumped();
    int justLanded();
    int justSkidded();
    int justFlipped();
    int justMoved();
    
    virtual void setPos(int,int,int);
    virtual void setPosAverage(int,int,int);
    virtual void setX(int);
    virtual void setY(int);
    virtual void setZ(int);

    int distToCube(CubeObj*);
    
    void changePos(int,int,int);
    void changeX(int);
    void changeY(int);
    void changeZ(int);
    void changePosTowards(CubeObj*,double);
    void changePosTowards(int, int, int, double);
     
    void setMomentumX(float);
    void setMomentumY(float);
    void setMomentumZ(float);
    void setMomentum(vector<float>);
    void setMomentumAverage(vector<float>);
    
    void resetToldToMove();
    void movePos(float,float,float);
    void moveX(float);
    void moveY(float);
    void moveZ(float);

    int get(int);
    int getX();
    int getY();
    int getZ();
    int getAngleX();
    int getAngleY();
    int getAngleZ();
    int getMomentumX();
    int getMomentumY();
    int getMomentumZ();
    vector<float> getMomentum();
    int getMomentumGround();
    int getMaxJump();
    bool getJustHitPlayer();
    void setJustHitPlayer(bool);
    
    void setNeighbors(bool,bool,bool,bool,bool,bool);
    void setVisibleNeighbors(bool,bool,bool,bool,bool,bool);
    void setVisibleNeighborObjects(CubeObj*,CubeObj*,CubeObj*,CubeObj*,CubeObj*,CubeObj*);
    bool* getNeighbors();
    bool* getVisibleNeighbors();
    CubeObj** getVisibleNeighborObjects();
    // These two check neighbor status and return if double neighbors in one or two dimensions respectively
    bool isColumn();
    bool isWall();
    bool isVertWall();
    
    void setEdges(bool,bool,bool,bool,bool,bool);
    bool* getEdges();
    void setCollision(bool);
    void applyCollisionMomentumX();
    void applyCollisionMomentumZ();
    bool getCollision();
    bool getDirectionConflict();
    float getDirectionConflictSeverity();
    virtual int getWidth();
    virtual int getHeight();
    virtual int getSize(int);
    int getMaterial();
    bool getAlternatingSpot();
    int getAlternatingSpotSize();
    bool getDuplicateNeighbor();
    void setMaterial(int);
    void setDuplicateNeighbor(bool);

    float getStrength();
    float getDirection();
    void setDirection(float);
    float getToldDirection();
    void setToldDirection(float);
    void setToldDirectionAverage(float);
    int getCamDirection(); // translated to camera numbers
    int getLandedOn();
    void setLandedOn(int);
    void setInvisible(bool);
    bool isInvisible();
    bool isItem();

    float myFpsRate();
    bool getLastToldToMove();
    int getMaxSpeed();
};

#endif 
