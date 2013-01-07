/*
 * cubeObj.h
 * by Brian Handy
 * 2/11/12
 * Header for any cube in the game (obstacles, items and players)
 */

#ifndef CUBEOBJ
#define CUBEOBJ

//#include "cameraObj.h"

class CubeObj {
  protected:
    static const int altSize = 400; // how wide the checker patterns are
    static const int maxMovement = 49; // less than half of a cube width
    static const int maxFall = 49; // less than half of a cube width
    bool fpsRateEnabled, duplicateNeighbor;
    bool neighbors[6],visibleNeighbors[6],edges[6],toldToMove,lastToldToMove;
    CubeObj* visibleNeighborObjects[6];
    float momentumX, momentumY, momentumZ, movementSpeed, movementDivision;
    bool toldToMoveX, toldToMoveY, toldToMoveZ;
    float toldToMoveXDist, toldToMoveYDist, toldToMoveZDist; // for immediate directions/instructions
    int x, y, z, diffX, diffY, diffZ, oldX, oldY, oldZ,
        landedOnX, landedOnY, landedOnZ, landedOnCount;
    bool hasMaterial, playerStatus, cameraStatus, newJump, loseMomentumOnLock;
    bool locked, lockable, permalocked, jumpable, grounded, lastGrounded, doubleLastGrounded, neighborsSet, visibleNeighborsSet;
    int maxSpeed, friction;
    float maxJump, jumpSpeed, jumpSpeedRatio, gravity;
    int material;
    bool jumping, lastJumping, collision, lastCollision;
    CubeObj* landedOn;
    //CameraObj* camera; // for camera cubes to ID their cameras
    float landedOnDirectionDiff, landedOnToldDirectionDiff;
    float newFriction, strength, direction, toldDirection; // for use with new friction technique
    bool invisible;
  public:
    CubeObj();

    void tick();

    void calculateDiff();
    void fall();
    void land();
    void landOn(CubeObj*);
    bool updateLandedOnPos();
    int getLandedOnCount();
    void freeze();
    
    // return status
    bool isMoving();
    bool isMovingQuickly();
    bool isPlayer();
    bool isCamera();
    void setCameraStatus(bool);
    //void setCamera(CameraObj*);
    //CameraObj* getCamera();

    void jump(bool);

    virtual void collisionEffect(CubeObj*);

    void setLock(bool);
    bool getLock();
    void setPermalock(bool);
    bool getPermalock();
    bool getGrounded();
    bool getStillGrounded();
    bool getNotGrounded();
    bool getLanded();
    bool justJumped();
    bool justBumped();

    void setPos(int,int,int);
    void setX(int);
    void setY(int);
    void setZ(int);

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
    int getMomentumGround();
    int getMaxJump();
    
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
    float getToldDirection();
    int getCamDirection(); // translated to camera numbers
    void setInvisible(bool b) { invisible = b; }
    bool isInvisible() { return invisible; }

    float myFpsRate();
    bool getLastToldToMove();
    int getMaxSpeed();
};

#endif 
