/*
 * cubeObj.h
 * by Brian Handy
 * 2/11/12
 * Header for any cube in the game (obstacles, items and players)
 */

#ifndef CUBEOBJ
#define CUBEOBJ

class CubeObj {
    int x, y, z, momentumX, momentumY, momentumZ;
    int movementSpeed;
    bool locked, lockable, jumpable, grounded, loseMomentumOnLock;
    int maxSpeed, friction;
    int maxJump, jumpSpeedRatio, floor, gravity;
  public:
    CubeObj();

    void tick();

    void fall();
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

    void movePos(int,int,int);
    void moveX(int);
    void moveY(int);
    void moveZ(int);

    int getX();
    int getY();
    int getZ();

};

#endif 