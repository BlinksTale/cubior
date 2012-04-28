// File to read map in and set gameplay as such
// modified from http://www.cplusplus.com/doc/tutorial/files/

// reading a text file
#include "mapReader.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

Map* MapReader::readMap(const string& s) {
  // To read file
  string file = s;
  string row;
  // To keep track of pos in file
  int iterator = 0;
  bool readingMap = false; // not until w/h/d are found
  bool widthFound = false;
  bool heightFound= false;
  bool depthFound = false;
  int w = 0;
  int h = 0;
  int d = 0;
  // To return map
  Map* map = new Map;

  ifstream newMap;
  newMap.open(file);
  if (newMap.is_open()) {
    while (newMap.good()) {
      getline(newMap, row);
      // Find initial properties
      if (!widthFound && !row.substr(0,6).compare("width:")) {
        cout << "Width found" << endl;
        widthFound = true;
        map->setWidth(10);
      }
      if (!heightFound && !row.substr(0,7).compare("height:")){
        cout << "Height found"<< endl;
        heightFound = true;
        map->setHeight(10);
      }
      if (!depthFound && !row.substr(0,6).compare("depth:")) {
        cout << "Depth found" << endl;
        depthFound = true;
        map->setDepth(10);
      }
      // Start the spot reading!
      if (!readingMap && widthFound && heightFound && depthFound) {
        readingMap = true;
      }
      if (readingMap) {
        if (!row.substr(0,0).empty()) {
          d=0;
          h++;
        } else {
          w = 0;
          while (w<row.length()) {
            if (!row.substr(w,1).compare("1")) {
              CubeObj newCube;
              newCube.tick();
              map->addCube(&newCube,w,h,d);
            }
            w++;
          }
          d++;
        }
      }
      cout << "on line " << iterator << endl;
      iterator++;
      cout << row << endl;
    }
    if (newMap.good()) {
      cout << "It failed!" << endl;
    }
    newMap.close();
  }

  else cout << "Unable to open " << file << endl; 

  return map;
}
