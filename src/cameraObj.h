/*
 * cameraObj.h
 * by Brian Handy
 * 3/29/12
 * Header for any camera in the game
 */

#ifndef CAMERAOBJ
#define CAMERAOBJ

class CameraObj {
  protected:
    int x, y, z;
    int player;
  public:
    CameraObj();
    void follow(int,int,int);
    int get(int);
    int getX();
    int getY();
    int getZ();
    void setPos(int,int,int);
    void setX(int);
    void setY(int);
    void setZ(int);
};

#endif 
