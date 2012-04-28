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

}

Map::Map(int w, int h, int d) {
  // new map!
  width = w;
  height = h;
  depth = d;
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
int Map::getWidth() { return width; }
int Map::getHeight(){ return height;}
int Map::getDepth() { return depth; }
CubeObj* Map::getCubeAt(int w, int h, int d) { return map[w][d][h]; }
