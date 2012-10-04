/*
 * Sound effects for Cubior
 * by Brian Handy
 * 10/03/12
 * Sfx class for 3d platformer
 * 
 * Sfx basics learned from online tutorial:
 * http://goanna.cs.rmit.edu.au/~gl/teaching/Interactive3D/courseware/tutorial9.html
 */

// Definitely need AL and ALUT stuff
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <cstdio> // for argv and argc


#include "sfx.h"
#include "gameplay.h"
// Not sure if I need any of these,
// but assuming so since this file
// should mimick gameplay.cpp
#include "cameraObj.h"
#include "cubeObj.h"
#include "goalObj.h"
#include "cubiorObj.h"
#include "collision.h"
#include "flatRender.h"

// Again, not sure on these either (gameplay copies)
#include <cmath> // for trig stuff with getAngleBetween
#include <iostream>
#include <cstdio>
#include <stdlib.h> // for NULL
#include <string> // for loading a level by var passed

// Sfx to use later, test for now, so named "source"
ALuint testSource;
ALuint jumpSource;
  
// Setup for sound effects
void initSfx(int argc, char** argv) {
  
  // Initialize like in GLUT
  alutInit(&argc, argv);
  
  // Load test sound
  /*ALuint testBuffer = alutCreateBufferFromFile("./sfx/Jump/Jump 3.wav");
  // Create source
  alSourcei(testSource, AL_BUFFER, testBuffer);
  alSourcei(testSource, AL_LOOPING, AL_TRUE); // AL_TRUE would make it loop
  */
  // Load sfx from file
  // Not sure why, but can't put alGenSources into createSfx and have it work
  // sound will not play if that's the case
  alGenSources(1, &testSource);
  alGenSources(1, &jumpSource);
  createSfx("./sfx/Jump/Jump 3.wav",testSource);
  createSfx("./sfx/Jump/Jump 3.wav",jumpSource);
  
}

// Main loop for sound effects, called once per cycle
void sfxLoop() {
  if (true) {
    playSfx(testSource);
  }
}

// Specific sound effects
void playTestSfx() {
  playSfx(testSource);
}
void playJumpSfx() {
  playSfx(jumpSource);
}

// Take in any source and create a sfx
void createSfx(const char* file, ALuint sfx) {
  
  ALuint buffer = alutCreateBufferFromFile(file);
  // Create Sfx Source
  //alGenSources(1, &sfx);
  alSourcei(sfx, AL_BUFFER, buffer);
  alSourcei(sfx, AL_LOOPING, AL_TRUE); // AL_TRUE would make it loop
}

// Take in any source and play the sfx
void playSfx(ALuint sfx) {
  // Play the sound effect! (yes, that one)
  alSourcePlay(sfx);
  // Then check for errors
  int error = alGetError();
  if (error)
      printf("%s\n", alutGetErrorString(error));
}