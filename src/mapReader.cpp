// File to read map in and set gameplay as such
// modified from http://www.cplusplus.com/doc/tutorial/files/

// reading a text file
#include "mapReader.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int MapReader::readMap(const string& s) {
  string file = s;
  string row;
  ifstream newMap;
  newMap.open(file);//../maps/cubiorMap0.cubior");
  if (newMap.is_open()) {
    while (newMap.good()) {
      getline(newMap, row);
      cout << row << endl;
    }
    if (newMap.good()) {
      cout << "It failed!" << endl;
    }
    newMap.close();
  }

  else cout << "Unable to open " << file << endl; 

  return 0;
}
