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
    int x, y, z, momentumX, momentumY, momentumZ;
    int movementSpeed, movementDivision;
    bool locked, lockable, jumpable, grounded, loseMomentumOnLock;
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

    void setLock(bool);
    bool getLock();

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

    int getX();
    int getY();
    int getZ();
    int getMomentumX();
    int getMomentumY();
    int getMomentumZ();

    virtual int getWidth();
    virtual int getHeight();
};

#endif 
