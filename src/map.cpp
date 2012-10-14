/*
 * map.cpp
 * by Brian Handy
 * 4/28/12
 * Concise map holder, keeps block population and sizes
 */

#include "map.h"
#include "cubeObj.h"
#include "gameplay.h"

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

// Default way to initialize a map
void Map::init() {
  goalDepth = 0;
  defaultColors();
}

void Map::addCube(CubeObj* cube, int w, int h, int d) {
  map[w][h][d] = cube;
}

void Map::removeCubeAt(int w, int h, int d) {
  delete map[w][h][d];
}

void Map::wipeMap() {
  // Have some size first
  if (width>0 && height>0 && depth>0) {
    // Then delete contents in all slots
    for (int w=0; w<width; w++) {
      for (int h=0; h<height; h++) {
        for (int d=0; d<depth; d++) {
          removeCubeAt(w,h,d);
        }
      }
    }
  }
}

void Map::setWidth(int n) { width = n; }
void Map::setHeight(int n){ height= n; }
void Map::setDepth(int n) { depth = n; }
void Map::setCubeCount(int n) { cubeCount = n; }
void Map::setGoalHeight(int n) { goalHeight = n; }
void Map::setGoalDepth(int n) { goalDepth = n; }
int Map::getWidth() { return width; }
int Map::getHeight(){ return height;}
int Map::getDepth() { return depth; }
int Map::getCubeCount() { return cubeCount; }
int Map::getGoalHeight() { return goalHeight; }
int Map::getGoalDepth() { return goalDepth; }
void Map::setCustomColors(float a, float b, float c) { red = a; green = b; blue = c; customColors = true; }
void Map::defaultColors() { red = 0.3; green = 1.0; blue = 1.0; }
CubeObj* Map::getCubeAt(int w, int h, int d) {
  return map[w][h][d];
}

// Background's solid color!
float Map::getRed()  { return red; }
float Map::getGreen(){ return green;}
float Map::getBlue() { return blue; }
