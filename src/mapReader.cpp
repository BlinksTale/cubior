// File to read map in and set gameplay as such
// modified from http://www.cplusplus.com/doc/tutorial/files/

// reading a text file
#include "mapReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>// for atoi
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
  bool goalHeightFound = false;
  bool haveRed   = false;
  bool haveGreen = false;
  bool haveBlue  = false;
  float red, green, blue;
  int w = 0;
  int h = 0;
  int d = 0;
  // To return map
  Map* map = new Map;
	
  // Stream in the file
  ifstream newMap;
  newMap.open(file.c_str());
	
  // Now find the ToC details
  if (newMap.is_open()) {
    while (newMap.good()) {
      getline(newMap, row);
      // Find initial properties
      if (!widthFound && !row.substr(0,6).compare("width:")) {
        widthFound = true;
        map->setWidth(atoi((row.substr(6,row.length()-6)).c_str()));
      }
      if (!heightFound && !row.substr(0,7).compare("height:")){
        heightFound = true;
        map->setHeight(atoi((row.substr(7,row.length()-7)).c_str()));
      }
      if (!depthFound && !row.substr(0,6).compare("depth:")) {
        depthFound = true;
        map->setDepth(atoi((row.substr(6,row.length()-6)).c_str()));
      }
      if (!goalHeightFound && !row.substr(0,11).compare("goalHeight:")) {
        goalHeightFound = true;
        map->setGoalHeight(atoi((row.substr(11,row.length()-11)).c_str()));
      }
      // Find background color
      if (!haveRed && !row.substr(0,4).compare("red:")) {
        haveRed = true;
        red = atof((row.substr(4,row.length()-4)).c_str());
      }
      if (!haveGreen && !row.substr(0,6).compare("green:")) {
        haveGreen = true;
        green = atof((row.substr(6,row.length()-6)).c_str());
      }
      if (!haveBlue && !row.substr(0,5).compare("blue:")) {
        haveBlue = true;
        cout << "Found at blue: " << (row.substr(5,row.length()-5)) << endl;
        blue = atof((row.substr(5,row.length()-5)).c_str());
        cout << "Making blue: " << blue << endl;
      }
      // Start the spot reading!
      if (row.length()==0 && !readingMap && widthFound && heightFound && depthFound) {
        readingMap = true;
        // But also check to see if we have a full set of new colors
        map->setCustomColors(red, green, blue);
        cout << "Blue is " << blue << " and red is " << red << " so green is " << green << endl;
      }
      if (readingMap) {
        if (row.length()==0) {
          d=0;
          h++;
        } else {
          w = 0;
          while (w<row.length()&&w<map->getWidth()) {
            // Convert map numbers to material numbers
            std::string mapColorString = row.substr(w,1);
            int mapColor = atoi(mapColorString.c_str());
            // Add any tile
            if (mapColor!=0) {
              CubeObj* newCube = new CubeObj();
              newCube->setMaterial(mapColor);
              newCube->tick();
              map->addCube(newCube,w,h,d);
              map->setCubeCount(map->getCubeCount()+1);
            }
            w++;
          }
          d++;
        }
      }
      iterator++;
    }
    if (newMap.good()) {
      cout << "It failed!" << endl;
    }
    newMap.close();
  }

  else cout << "Unable to open " << file << endl; 
	
  return map;
}
