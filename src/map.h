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
    int width, height, depth, cubeCount, goalHeight, goalDepth;
    bool customColors;
    float red, green, blue;
  public:
    Map();
    Map(int,int,int);
    void init();

    void addCube(CubeObj*,int,int,int);
    void removeCubeAt(int,int,int);
    void wipeMap();

    void setWidth(int);
    void setHeight(int);
    void setDepth(int);
    void setCubeCount(int);
    void setGoalHeight(int);
    void setGoalDepth(int);
    void setCustomColors(float,float,float);
    void defaultColors();

    int getWidth();
    int getHeight();
    int getDepth();
    int getCubeCount();
    int getGoalHeight();
    int getGoalDepth();
    float getRed();
    float getGreen();
    float getBlue();
    CubeObj* getCubeAt(int,int,int);
};

#endif 
