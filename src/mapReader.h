/*
 * mapReader.h
 * by Brian Handy
 * 4/27/12
 * Header for the .cubior to map converter
 */

#ifndef MAPREADER
#define MAPREADER

#include <string>
#include "map.h"

class MapReader {
  public:
    static void init();
    static Map* readMap(const std::string&);
    static void fillSpotWithInvisible(Map*,int,int,int);
    static int getPadding() { return padding; }
};

#endif 
