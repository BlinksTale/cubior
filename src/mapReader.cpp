// File to read map in and set gameplay as such
// modified from http://www.cplusplus.com/doc/tutorial/files/

// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int readMap(string s) {
  string row;
  ifstream newMap("../maps/cubiorMap0.cubior");
  if (newMap.is_open()) {
    while (newMap.good()) {
      getline(newMap, row);
      cout << row << endl;
    }
    newMap.close();
  }

  else cout << "Unable to open file"; 

  return 0;
}