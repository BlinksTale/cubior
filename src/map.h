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
#ifdef __APPLE__
#include <tr1/unordered_map>
#else
#include <unordered_map>
#endif
//#include "gameplay.h"

using namespace std;
using namespace std::tr1;

class Map {
  protected:
    CubeObj* map[maxWidth][maxHeight][maxDepth];
    unordered_map<int, CubeObj*> map_unordered;
    int width, height, depth, cubeCount, goalWidth, goalHeight, goalDepth;
    bool customColors, empty, exists;
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
    void setGoalPosition(int,int,int);
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
    int getSlot(int,int,int);
};

#endif 
