/*
 * Music for Cubior
 * by Brian Handy
 * (Music itself by Waterflame)
 * 10/12/12
 * Music class for 3d platformer
 */


#ifndef MUSIC
#define	MUSIC

#include <cstdio> // for argv and argc
#include <SFML/Audio.hpp> // for sound manipulation

#include "gameplay.h"
//#include "flatRender.h" // pretending I don't need this for now

  void initMusic(int, char**);
  void musicLoop();

#endif