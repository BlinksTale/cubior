/*
 * map.h
 * by Brian Handy
 * 4/28/12
 * Maps are a quick storage for the pos of all cubes in a level
 */

#ifndef MAP
#define MAP

#include "constraints.h" // for max width/depth/height
#include "cubeObj.h"
//#include "gameplay.h"

class Map {
  protected:
    CubeObj* map[maxWidth][maxHeight][maxDepth];
    int width, height, depth, cubeCount, goalWidth, goalHeight, goalDepth;
    bool customColors;
    float red, green, blue;
  public:
    Map();
    Map(int,int,int);
    ~Map();
    void init();

    void addCube(CubeObj*,int,int,int);
    void removeCubeAt(int,int,int);
    void wipeMap();
    bool isSurrounded(int,int,int);
    void wipeSurrounded();

    void setWidth(int);
    void setHeight(int);
    void setDepth(int);
    void setCubeCount(int);
    void setGoalWidth(int);
    void setGoalHeight(int);
    void setGoalDepth(int);
    void setCustomColors(float,float,float);
    void defaultColors();

    int getWidth();
    int getHeight();
    int getDepth();
    int getCubeCount();
    int getGoalWidth();
    int getGoalHeight();
    int getGoalDepth();
    float getRed();
    float getGreen();
    float getBlue();
    CubeObj* getCubeAt(int,int,int);
};

#endif 
