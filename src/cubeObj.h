/*
 * cubeObj.h
 * by Brian Handy
 * 2/11/12
 * Header for any cube in the game (obstacles, items and players)
 */

#ifndef CUBEOBJ
#define CUBEOBJ

class CubeObj {
  protected:
    bool neighbors[6],edges[6];
    int x, y, z, momentumX, momentumY, momentumZ, diffX, diffY, diffZ, oldX, oldY, oldZ;
    int movementSpeed, movementDivision;
    bool collision, hasMaterial, playerStatus, newJump;
    bool locked, lockable, permalocked, jumpable, grounded, lastGrounded, loseMomentumOnLock;
    int maxSpeed, friction;
    int maxJump, jumpSpeedRatio, gravity;
    int material;
  public:
    CubeObj();

    void tick();

    void calculateDiff();
    void fall();
    void land();
    void freeze();
    bool isMoving();
    bool isMovingQuickly();
    bool isPlayer();
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
     
    void setMomentumX(int);
    void setMomentumY(int);
    void setMomentumZ(int);
    
    void movePos(int,int,int);
    void moveX(int);
    void moveY(int);
    void moveZ(int);

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

    void setNeighbors(bool,bool,bool,bool,bool,bool);
    bool* getNeighbors();
    // These two check neighbor status and return if double neighbors in one or two dimensions respectively
    bool isColumn();
    bool isWall();
    bool isVertWall();
    
    void setEdges(bool,bool,bool,bool,bool,bool);
    bool* getEdges();
    void setCollision(bool);
    bool getCollision();
    virtual int getWidth();
    virtual int getHeight();
    virtual int getSize(int);
    int getMaterial();
    void setMaterial(int);
};

#endif 
