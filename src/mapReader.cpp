// File to read map in and set gameplay as such
// modified from http://www.cplusplus.com/doc/tutorial/files/

// reading a text file
#include "mapReader.h"
#include "gameplay.h"
#include "springObj.h"
#include "crumblingObj.h"
#include "movingObj.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>// for atoi
#include <stdio.h> // for fopen
using namespace std;

#define NULL 0

bool mapMade = false;
Map* map = NULL;

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
  int foundWidth = 0;
  int foundHeight = 0;
  int foundDepth = 0;
  int foundGoalWidth = 0;
  int foundGoalHeight = 0;
  int foundGoalDepth = 0;
  bool goalWidthFound = false;
  bool goalHeightFound = false;
  bool goalDepthFound = false;
  bool haveRed   = false;
  bool haveGreen = false;
  bool haveBlue  = false;
  float red, green, blue;
  int w = padding;
  int h = padding;
  int d = padding;
  // To return map

  #ifdef __APPLE_CC__
  #else 
  if (map != NULL) {
    // Alright, so… this breaks the game for Macs
    // so even though it looks like a memory leak otherwise,
    // to get the game functioning right now, we can only delete
    // the old map on Windows. And we must, or it breaks.
    //delete map;
  }
  #endif
  
  //map = new Map;//()
  mapMade = true;
	
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
        foundWidth = 2*padding+atoi((row.substr(6,row.length()-6)).c_str());
        //map->setWidth(foundWidth);
      }
      if (!heightFound && !row.substr(0,7).compare("height:")){
        heightFound = true;
        foundHeight = 2*padding+atoi((row.substr(7,row.length()-7)).c_str());
        //map->setHeight(foundHeight);
      }
      if (!depthFound && !row.substr(0,6).compare("depth:")) {
        depthFound = true;
        foundDepth = 2*padding+atoi((row.substr(6,row.length()-6)).c_str());
        //map->setDepth(foundDepth);
      }
      if (!goalWidthFound && !row.substr(0,10).compare("goalWidth:")) {
        goalWidthFound = true;
        foundGoalWidth = atoi((row.substr(10,row.length()-10)).c_str());
        //map->setGoalWidth(foundGoalWidth);
      }
      if (!goalHeightFound && !row.substr(0,11).compare("goalHeight:")) {
        goalHeightFound = true;
        foundGoalHeight = atoi((row.substr(11,row.length()-11)).c_str());
        //map->setGoalHeight(foundGoalHeight);
      }
      if (!goalDepthFound && !row.substr(0,10).compare("goalDepth:")) {
        goalDepthFound = true;
        foundGoalDepth = atoi((row.substr(10,row.length()-10)).c_str());
        //map->setGoalDepth(foundGoalDepth);
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
        blue = atof((row.substr(5,row.length()-5)).c_str());
      }
      // Start the spot reading!
      // (read in the cubes from their individual spots)
      if (row.length()==0 && !readingMap && widthFound && heightFound && depthFound) {
        readingMap = true;
        // And in beginning to read the map,
        // also assign the map var with a new map of the appropriate size
        map = new Map(foundWidth, foundHeight, foundDepth);

        // Then try to set each of the goal position dimensions
        if (foundGoalWidth) {
          map->setGoalWidth(foundGoalWidth);
        }
        if (foundGoalHeight) {
          map->setGoalHeight(foundGoalHeight);
        }
        if (foundGoalDepth) {
          map->setGoalDepth(foundGoalDepth);
        }
        
        // But also check to see if we have a full set of new colors
        if (haveRed && haveGreen && haveBlue) {
          map->setCustomColors(red, green, blue);
        }
      }
      if (readingMap) {
        if (row.length()==0) {
          d=padding;
          h++;
        } else {
          w = padding;
          while (w-padding<row.length()&&w<map->getWidth()-padding) {
            // Convert map numbers to material numbers
            std::string mapColorString = row.substr(w-padding,1);
            int mapColor = atoi(mapColorString.c_str());
            // Add any tile
            if (mapColor!=0) {
              // Regular Environment/Ground Tiles
              CubeObj* newCube = new CubeObj();
              newCube->setMaterial(mapColor);
              newCube->tick();
              newCube->setPermalock(true);
              map->addCube(newCube,w,h,d);
            } else if (mapColorString.c_str()[0] == 'S') {
                CubeObj* newCube = new SpringObj();
                //newCube->setMaterial(mapColor);
                newCube->tick();
                newCube->setPermalock(true);
                map->addCube(newCube,w,h,d);
            } else if (mapColorString.c_str()[0] == 'C') {
                CubeObj* newCube = new CrumblingObj();
                //newCube->setMaterial(mapColor);
                newCube->tick();
                newCube->setPermalock(true);
                map->addCube(newCube,w,h,d);
            } else if (mapColorString.c_str()[0] == 'M') {
                CubeObj* newCube = new MovingObj();
                newCube->tick();
                // moving objects are not permalocked, their position updates
//                newCube->setPermalock(true);
                map->addCube(newCube,w,h,d);
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

  int newWidth = foundWidth;
  int newHeight = foundHeight;
  int newDepth = foundDepth;
  int altWidth = map->getWidth();
  int altHeight = map->getHeight();
  int altDepth = map->getDepth();

  // Finally, just before returning it, add invisible walls
  // Z wallz first
  for (int w=0; w<newWidth; w++) {
    for (int h=0; h<newHeight; h++) {
      // Rear Wall
      for (int d=0; d<padding; d++) {
        fillSpotWithInvisible(map,w,h,newDepth-padding+d); // THIS IS CORRECT ON LEVEL 0
      }
      // Front Wall
      for (int d=0; d<padding; d++) {
        fillSpotWithInvisible(map,w,h,d);
      }
    }
  }
  // Then X walls
  for (int d=0; d<newDepth; d++) {
    for (int h=0; h<newHeight; h++) {
      // Right Wall
      for (int w=0; w<padding; w++) {
        fillSpotWithInvisible(map,newWidth-padding+w,h,d);
      }
      // Left Wall
      for (int w=0; w<padding; w++) {
        fillSpotWithInvisible(map,w,h,d);
      }
    }
  }
  // Finally, get rid of any extra cubes (totally enclosed in other cubes)
  //map->wipeSurrounded();
  //map->wipeMap();
  // alright, so wipeSurrounded REALLY doesn't work right now. Try moving it to gameplay?

  // Then return it!
  return map;
}

// Put invisible blot in a spot, or make current block invisible
void MapReader::fillSpotWithInvisible(Map* map, int spotX, int spotY, int spotZ) {
  CubeObj* focus = map->getCubeAt(spotX,spotY,spotZ);
  bool isInbounds = map->getInbounds(spotX,spotY,spotZ);
  if (focus != NULL && isInbounds) {
    if (!focus->isInvisible()) {
      focus->setInvisible(true);
      focus->setMaterial(8);
      focus->tick();
    }
  } else {
    CubeObj* newCube = new CubeObj();
    newCube->setInvisible(true);
    newCube->setMaterial(3);
    newCube->setPermalock(true);
    newCube->tick();
    map->addCube(newCube,spotX,spotY,spotZ);
  }
}
