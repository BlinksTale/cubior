/*
 * map.cpp
 * by Brian Handy
 * 4/28/12
 * Concise map holder, keeps block population and sizes
 */

#include "map.h"
#include "cubeObj.h"
#include "gameplay.h"
#include <unordered_map>

#define oldMap true

Map::Map() {
  init();
}

Map::Map(int w, int h, int d) {
  // new map!
  width = w;
  height = h;
  depth = d;
  init();
}

Map::~Map() {
  wipeMap();
}

// Default way to initialize a map
void Map::init() {
  goalWidth = 0;
  goalDepth = 0;
  goalDepth = 0;
  cubeCount = 0;

  defaultColors();

  exists = true;
}

void Map::addCube(CubeObj* cube, int w, int h, int d) {
  #ifdef oldMap
    map[w][h][d] = cube;
  #else
    int slot = getSlot(w, d, h);
    map_unordered[slot] = cube;*/
  #endif
  cubeCount++;
}

void Map::removeCubeAt(int w, int h, int d) {
  if (width > 0 && height > 0 && depth > 0 &&
      w >= 0 & w < width  &&
      h >= 0 & h < height &&
      d >= 0 & d < depth  ) {
    #ifdef oldMap
      //if (map[w][h][d] != 0) {
        delete map[w][h][d];
        map[w][h][d] = NULL;
        cubeCount--;
      //}
    #else
    //if (map_unordered[slot] != NULL) {
      // Look if slot exists or not
      int slot = getSlot(w, h, d);
      if (map_unordered.find(slot) != map_unordered.end() && map_unordered[slot]) {
        delete map_unordered[slot];
        cubeCount--;
      }
    #endif
  }
}

void Map::wipeMap() {
  //if (!empty) {
  // Have some size first
  if (exists && width>0 && height>0 && depth>0) {
    // Then delete contents in all slots
    for (int w=0; w<width; w++) {
      for (int h=0; h<height; h++) {
        for (int d=0; d<depth; d++) {
          removeCubeAt(w,h,d);
        }
      }
    }
  }
  cubeCount = 0;
  //empty = true;
  //}
}

bool Map::isSurrounded(int cX, int cY, int cZ) {
  // Map edges are never surrounded
  if (cX < 1 || cY < 1 || cZ < 1 ||
      cX > width-1 || cY > height-1 || cZ > depth-1) {
        return false;
  } else {
    // Find neighbors
    bool n0 = map[cX+1][cY][cZ] != 0;
    bool n1 = map[cX-1][cY][cZ] != 0;
    bool n2 = map[cX][cY+1][cZ] != 0;
    bool n3 = map[cX][cY-1][cZ] != 0;
    bool n4 = map[cX][cY][cZ+1] != 0;
    bool n5 = map[cX][cY][cZ-1] != 0;
    // All full?
    return n0 && n1 && n2 && n3 && n4 && n5;
  }
}
void Map::wipeSurrounded() {
  //cout << "Wiping " << width << ", " << height << ", " << depth << endl;
  // Have some size first
  if (exists && width>0 && height>0 && depth>0) {
    // Then delete contents in all slots
    for (int cX=1+padding; cX<width-padding-1; cX++) {
      for (int cY=1+padding; cY<height-padding-1; cY++) {
        for (int cZ=1+padding; cZ<depth-padding-1; cZ++) {
          // All full? Delete self
          if (isSurrounded(cX,cY,cZ)) {
            removeCubeAt(cX,cY,cZ);
          }
        }
      }
    }
  }
}

void Map::setWidth(int n) { width = n; }
void Map::setHeight(int n){ height= n; }
void Map::setDepth(int n) { depth = n; }
void Map::setCubeCount(int n) { cubeCount = n; }
void Map::setGoalPosition(int n, int o, int p) {
  goalWidth = n;
  goalHeight = o;
  goalDepth = p;
}
void Map::setGoalWidth(int n) { goalWidth = n; }
void Map::setGoalHeight(int n){ goalHeight= n; }
void Map::setGoalDepth(int n) { goalDepth = n; }
int Map::getWidth() { return width; }
int Map::getHeight(){ return height;}
int Map::getDepth() { return depth; }
int Map::getCubeCount() { return cubeCount; }
int Map::getGoalWidth() { return goalWidth; }
int Map::getGoalHeight(){ return goalHeight;}
int Map::getGoalDepth() { return goalDepth; }
void Map::setCustomColors(float a, float b, float c) { red = a; green = b; blue = c; customColors = true; }
void Map::defaultColors() { red = 0.3; green = 1.0; blue = 1.0; }
CubeObj* Map::getCubeAt(int w, int h, int d) {
  int slot = getSlot(w, d, h);
  if (
      w >= 0 && w < width  &&
      h >= 0 && h < height &&
      d >= 0 && d < depth ) {
    #ifdef oldMap
      return map[w][h][d];
    #else
      if (exists && map_unordered[slot] != NULL) {
        return map_unordered[slot];
      }
    #endif
  } else {
    return NULL;
  }
}

// Background's solid color!
float Map::getRed()  { return red; }
float Map::getGreen(){ return green;}
float Map::getBlue() { return blue; }

int Map::getSlot(int w, int d, int h) {
  return 1000*1000*w + 1000*d + h;
}