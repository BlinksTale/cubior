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

#include "gameplay.h"
//#include "flatRender.h" // pretending I don't need this for now

  void initMusic(int, char**);
  void musicLoop();
  void playSong(int);
  void nextSong();
  void setMute(bool);
  void toggleMute();
  void increaseMusicVolume();
  void decreaseMusicVolume();
  void cycleMusicVolume();
  int getMusicVolume();
  int getMusicVolumeNum();

#endif