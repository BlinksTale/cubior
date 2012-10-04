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
ALuint testSource, exitSource, bumpSource, errorSource,
        menuEnterSource, menuExitSource;
ALuint jumpSource[4];
  
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
  /* My thoughts on the Rolando Nadal SFX so far

     I checked them all out - way more than I was expecting for a first batch!
     Definitely liked them overall, probably miscommunicated what "hit/collision"
     was supposed to be (was looking for bump-against-wall sfx rather than hit-by-an-enemy,
     so, something calmer/smoother to be heard as regularly as jumping) but I'll
     definitely try these out ingame and see how they work. Might need more
     instruction on what goes where though. Anyways, here were my thoughts as I 
     went through them! (started at Menu since Win Media Player wasn't playing
     the .wavs, Quicktime worked though)

     Error 1, 2, 3 - alright, makes sense
     Leave 1 - for finishing a level? This sounds cool 
     Leave 2,3,4,5 - all sound good/normal/make sense? I like that they're not
       just pitch changes... I think, there is more than just that in
       differences, right?
     Leave 6 - sounds cool, is that an echo or something that makes it sound
       different?
     Leave 7, 8 - can't tell difference between these two, also good. Reminds me
       of Megaman entrances

     Definitely liked the Leave sound effects overall! And Error gets the job 
       done, may even use as "go back" sfx. Was Leave for the Esc/Pause menu?
       It could also work for finishing a level.

     Jump 1 - Hah! It's bubbly, and entertaining 
     Jump 2 - Maybe too bubbly?
     Jump 3 - Sounds better, wish it could be longer and still sound like this 
       (it can't sound bubbly with how short it is now... some way to extend that?)
       Trying to figure this out... it's like the pitch doesn't change, or it's
       the right tone, whereas 2 is too high pitched or the pitch changes too
       much. Anyways, yeah, 3 sounds a lot better, can it be longer?
     Jump 4 - Hah! This is also cool! Reminds me of Kid Icarus
     Jump 5 - actually... I'd be interested to hear this one in a more bubbily/
       boo-wip sound, maybe mixed with 2 or something?
     Jump 6 - same, not sure which pitch I prefer now
     Jump 7 - ohhh, but it comes off a little strong. But I'd love to try it and
       see how that works out
     Jump 8 - Almost sounds better for an upgrade? But will definitely try it 
       ingame, I'm liking this so far
     Jump 9 - again, too bubbly. Wish I had a better word for this, sorry!
     Jump 10 - bubbly but more manageable than 9. Still not really one I'd use
       for jumping I think, maybe it sounds rounder from the lower pitch/tone/
       note/augh-music-words

     Overall, liked the diversity in jumping sounds, but didn't hear what I
     expected. Might work out great though, need to try it ingame - especially
     with longer versions of these if I can, to really know what it feels like
     to hold the jump button down for a full/long jump

     Hit 1 - ohh, too sharp, sorry
     Hit 2 - maybe these would work for getting damage?
     Hit 3 - I mean, it is a good collision sound, but like a hard strike, like
       digging into stone with a pickaxe or something. This would really be a
       "bump" I'm looking for then, when Cubior hits a wall. I don't want a SFX
       this painful for average navigation, just looking for a cue to signal you
       touching a surface. Might work for falling from long heights
     Hit 4,5,6 - more of the same, five sounds nicer since it's softer, six even
       more than that
     Hit 7 - too sharp again
     Hit 8,9 - Ooh! This is cool! And nine is the louder form?
     Hit 10 - Sounds more like being damaged and being pushed? Trying to figure
       out where this would work
     Hit 11 - almost better for projectile firing at this point
     Hit 12 - squirrel hopping? Might even work better with jumping from here.
     Hit 13 - silly sounding, slightly annoying xD it reminds me of the sfx used
       to jump on NPCs and silliness like that, eg. a toad's head in Super Mario
       Galaxy
     Hit 14 - almost villainous! Fire being shot out of an enemy's head?

     I think my gaming background is betraying me! I keep hearing strange things
     in here, but I didn't communicate hit properly. I don't have a plan for
     Cubior having health right now, nor even a damage system yet - but those
     might still exist! For now though, the IGF submission will most likely
     have no death, just onwards progress. Although maybe that's not challenging
     enough?

     Still, collision sounds for now would just be applied to touching walls and
     landing on the ground, or bumping into another guy (harmlessly pushing him
     forward/aside). I'd love to include enemies, but thats not in the scope I
     have planned for the next month. Camera, menu, goal touching cutscene and
     playtesting/level design are my current targets, so I don't think we'll be
     seeing health any time soon.
   */
  
  // Load sfx from file
  // Not sure why, but can't put alGenSources into createSfx and have it work
  // sound will not play if that's the case
  alGenSources(1, &testSource);
  for (int i=0; i<4; i++) {
    alGenSources(1, &(jumpSource[i]));
  }
  alGenSources(1, &exitSource);
  alGenSources(1, &bumpSource);
  alGenSources(1, &menuEnterSource);
  alGenSources(1, &menuExitSource);
  alGenSources(1, &errorSource);
  createSfx("./sfx/Jump/Jump 3.wav",testSource);
  createSfx("./sfx/Jump/Jump 2.wav",jumpSource[0]); // 2 or 3 so far. 6?
  createSfx("./sfx/Jump/Jump 3.wav",jumpSource[1]); // 2 or 3 so far. 6?
  createSfx("./sfx/Jump/Jump 9.wav",jumpSource[2]); // 2 or 3 so far. 6?
  createSfx("./sfx/Jump/Jump 10.wav",jumpSource[3]); // 2 or 3 so far. 6?
    // 9 and 10 good for sad cubes
  createSfx("./sfx/Menu-Game/Leave 8.wav",exitSource); // draw this one out
  createSfx("./sfx/Collision/Hit 6.wav",bumpSource);
  createSfx("./sfx/Menu-Game/Leave 3.wav",menuEnterSource);
  createSfx("./sfx/Menu-Game/Leave 5.wav",menuExitSource);
  createSfx("./sfx/Menu-Game/Error 3.wav",errorSource);
  
  // And finally, tweak volume for those that are too loud
	alSourcef(jumpSource[2], AL_GAIN, 0.4);
	alSourcef(jumpSource[3], AL_GAIN, 0.4);
}

// Main loop for sound effects, called once per cycle
void sfxLoop() {
  // Jumping SFX
  for (int i=0; i<cubiorCount; i++) {
    if (getCubiorJustJumped(i)) { playSfx(jumpSource[i]); }
  }
}

// Specific sound effects
void playTestSfx() {
  playSfx(testSource);
}
void playJumpSfx(int i) {
  playSfx(jumpSource[i]);
}
void playExitSfx() {
  playSfx(exitSource);
}
void playBumpSfx() {
  playSfx(bumpSource);
}
void playMenuEnterSfx() {
  playSfx(menuEnterSource);
}
void playMenuExitSfx() {
  playSfx(menuExitSource);
}
void playErrorSfx() {
  playSfx(errorSource);
}


// Take in any source and create a sfx
void createSfx(const char* file, ALuint sfx) {
  
  ALuint buffer = alutCreateBufferFromFile(file);
  // Create Sfx Source
  //alGenSources(1, &sfx);
  alSourcei(sfx, AL_BUFFER, buffer);
  alSourcei(sfx, AL_LOOPING, AL_FALSE); // AL_TRUE would make it loop
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