/*
 * Sound effects header for Cubior
 * by Brian Handy
 * 10/03/12
 * Sfx class for 3d platformer
 */

#ifndef SFX
#define	SFX

// Definitely need AL and ALUT stuff
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <cstdio> // for argv and argc

#include "gameplay.h"
//#include "flatRender.h" // pretending I don't need this for now

  void initSfx(int, char**);
  void sfxLoop();
  void playTestSfx();
  void playJumpSfx();
  void playExitSfx();
  void playBumpSfx();
  void playMenuEnterSfx();
  void playMenuExitSfx();
  void playErrorSfx();
  void createSfx(const char*, ALuint);
  void playSfx(ALuint);

#endif