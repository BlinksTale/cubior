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
    bool neighbors[6];
    int x, y, z, momentumX, momentumY, momentumZ;
    int movementSpeed, movementDivision;
    bool collision;
    bool locked, lockable, permalocked, jumpable, grounded, loseMomentumOnLock;
    int maxSpeed, friction;
    int maxJump, jumpSpeedRatio, floor, gravity;
  public:
    CubeObj();

    void tick();

    void fall();
    void land();
    void freeze();
    bool moving();
    void jump(bool);

    virtual void collisionEffect(CubeObj*);

    void setLock(bool);
    bool getLock();
    void setPermalock(bool);
    bool getPermalock();
    bool getGrounded();
    bool getLanded();

    void setPos(int,int,int);
    void setX(int);
    void setY(int);
    void setZ(int);

    void changePos(int,int,int);
    void changeX(int);
    void changeY(int);
    void changeZ(int);

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
    int getMomentumX();
    int getMomentumY();
    int getMomentumZ();

    void setNeighbors(bool,bool,bool,bool,bool,bool);
    bool* getNeighbors();
    void setCollision(bool);
    bool getCollision();
    virtual int getSize(int);
    virtual int getWidth();
    virtual int getHeight();
};

#endif 
