/*
 * map.h
 * by Brian Handy
 * 4/28/12
 * Maps are a quick storage for the pos of all cubes in a level
 */

#ifndef MAP
#define MAP

#include "cubeObj.h"
#include "gameplay.h"

class Map {
  protected:
    CubeObj* map[maxWidth][maxHeight][maxDepth];
    int width, height, depth, cubeCount;
  public:
    Map();
    Map(int,int,int);

    void addCube(CubeObj*,int,int,int);
    void removeCubeAt(int,int,int);

    void setWidth(int);
    void setHeight(int);
    void setDepth(int);
    void setCubeCount(int);

    int getWidth();
    int getHeight();
    int getDepth();
    int getCubeCount();
    CubeObj* getCubeAt(int,int,int);
};

#endif 
