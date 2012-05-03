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
  defaultColors();
}

Map::Map(int w, int h, int d) {
  // new map!
  width = w;
  height = h;
  depth = d;
  defaultColors();
}

void Map::addCube(CubeObj* cube, int w, int h, int d) {
  map[w][h][d] = cube;
}

void Map::removeCubeAt(int w, int h, int d) {
  map[w][h][d] = 0;
}

void Map::setWidth(int n) { width = n; }
void Map::setHeight(int n){ height= n; }
void Map::setDepth(int n) { depth = n; }
void Map::setCubeCount(int n) { cubeCount = n; }
void Map::setGoalHeight(int n) { goalHeight = n; }
int Map::getWidth() { return width; }
int Map::getHeight(){ return height;}
int Map::getDepth() { return depth; }
int Map::getCubeCount() { return cubeCount; }
int Map::getGoalHeight() { return goalHeight; }
void Map::setCustomColors(float a, float b, float c) { red = a; green = b; blue = c; customColors = true; }
void Map::defaultColors() { red = 0.3; green = 0.5; blue = 1.0; }
CubeObj* Map::getCubeAt(int w, int h, int d) {
  return map[w][h][d];
}

// Background's solid color!
float Map::getRed()  { return red; }
float Map::getGreen(){ return green;}
float Map::getBlue() { return blue; }
