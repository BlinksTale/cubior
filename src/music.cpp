/*
 * Music for Cubior
 * by Brian Handy
 * (Music itself by Waterflame)
 * 10/12/12
 * Music class for 3d platformer
 */

#include <cstdio> // for argv and argc
#include <SFML/Audio.hpp> // for sound manipulation

#include "music.h"
#include "gameplay.h"

// Again, not sure on these either (gameplay copies)
#include <cmath> // for trig stuff with getAngleBetween
#include <iostream>
#include <cstdio>
#include <stdlib.h> // for NULL
#include <string> // for loading a level by var passed

// Buffer Vars (hold song data)
sf::Music music;
bool mute = true;
bool lastMute = mute;

// List of songs
string songs[] = { "./music/Waterflame_MakeARunForIt+Cats+Orange.ogg" };//,
  //"./music/Waterflame_MakeARunForIt.ogg",
  //                 "./music/Waterflame_Cats.ogg",
  //                 "./music/Waterflame_Orange.ogg"};
int songCount = 1;
int currentSong = 0;

// Setup for sound effects
void initMusic(int argc, char** argv) {
  playSong(currentSong);
}

void playSong(int i) {
  // Load sounds into buffers
  music.openFromFile(songs[i]);
  music.setLoop(true);
  music.setVolume(70);
  
  // And setup sound players
  if (!mute) { music.play(); }
}

// Main loop for sound effects, called once per cycle
void musicLoop() {
  // Just unmuted? Play!
  if (!mute && lastMute) {
    music.play();
  }

  // Finally, if needed, update lastMute
  if (lastMute != mute) {
    lastMute = mute;
  }
}

void setMute(bool b) {
  mute = b;
}

void toggleMute() {
  mute = !mute;
}

void nextSong() {
  // Push song up past song count, but if already there, loop
  if (currentSong == songCount) {
    currentSong = 0;
  } else {
    currentSong++;
  }
  // If past song count (or equal to it) play no music
  // Otherwise, start the next song!
  if (currentSong == songCount) {
    music.stop();
  } else {
    playSong(currentSong);
  }
}