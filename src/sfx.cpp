/*
 * Sound effects for Cubior
 * by Brian Handy
 * 10/03/12
 * Sfx class for 3d platformer
 */

#include <cstdio> // for argv and argc
#include <SFML/Audio.hpp> // for sound manipulation

#include "sfx.h"
#include "gameplay.h"

// Again, not sure on these either (gameplay copies)
#include <cmath> // for trig stuff with getAngleBetween
#include <iostream>
#include <cstdio>
#include <stdlib.h> // for NULL
#include <string> // for loading a level by var passed

#include "ResourcePath.hpp" // to load (in XCode for Mac) from app's resource folder using the SFML file (combined with ResourcePath.mm)

// Basis for all volumes
int highVolumeSfx = 100;
int medVolumeSfx = 66;
int lowVolumeSfx = 33;
int offVolumeSfx = 0;
int baseVolume = highVolumeSfx;

// Buffer Vars (hold song data)
sf::SoundBuffer testBuffer, exitBuffer, errorBuffer,
  menuEnterBuffer, menuExitBuffer, glowBuffer,
  changeMenuBuffer, changeOptionBuffer,
  focusCameraBuffer, turnCameraBuffer, failCameraBuffer;
sf::SoundBuffer jumpBuffer[4], bumpBuffer[4], landBuffer[4],
  skidBuffer[4], flipBuffer[4], moveBuffer[4];

// Sound Vars (play what's in buffer)
sf::Sound testSound, exitSound, errorSound,
  menuEnterSound, menuExitSound, glowSound,
  changeMenuSound, changeOptionSound,
  focusCameraSound, turnCameraSound, failCameraSound;
sf::Sound jumpSound[4], bumpSound[4], landSound[4],
  skidSound[4], flipSound[4], moveSound[4];

// Sound muffling variables
bool sfxLastPlayerVisible[4];
int jumpVolumeMultiplier[] = {100,100,100,100};
int landVolume[4], skidVolume[4], flipVolume[4], moveVolume[4];

// Setup for sound effects
void initSfx(int argc, char** argv) {
  
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

     ---

     New batch from 10/27/12!

     Bump 1 is solid
     Bump 2 could not be heard :\
     Bump 3 is a more happy bump
     Bump 4 is a mid bump
     Bump 5 is dark
     Bump 6 is mid-to-happy
     Bump 7 == Bump 6?
     Bump 8 == Bump 4?
     Bump 9 is solid-to-dark
     Bump 10 is a hair darker than dark
     Bump 11 is awk solid?
     Bump 12 is awk mid-to-happy or happy

     Thinking...
     Bump 6 for p1
     Bump 4 for p2
     Bump 1 for p3
     Bump 5 for p4
     Also considering range Bump 3, Bump 6, Bump 1, Bump 5

     Goal 3 sounds best, long and lots happening

     Turn 1 or 4 for camera turns?
     Turn 10 for focus behind?
     Turn 6 would be good for p1 camera new angle
     Turn 5 would be good for p2 camera new angle
     Turn 1 would be good for p3 camera new angle
     Turn 2 would be good for p4 camera new angle
     Turn slow 5 for p1 flip around
     Turn slow 6 for p2 flip around
     Turn slow 3 for p3 flip around
     Turn slow 1 for p4 flip around

     Menu 1 good for changing menu option
     Menu 4 is good for changing menu itself
   */

  /*************/
  /* SFML Init */
  /*************/
    
  // Extra path ensures Resources folder is accessed for .app on Mac
  std::string extraPath = "";
  #ifdef __APPLE_CC__
    extraPath = resourcePath();
  #endif  
    
  // Load sounds into buffers
  testBuffer.loadFromFile(        extraPath + "./sfx/Jump/Jump 3 Long.wav");
  exitBuffer.loadFromFile(        extraPath + "./sfx/Menu-Game/Leave 8.wav");
  glowBuffer.loadFromFile(        extraPath + "./sfx/Menu-Game/Goal Echo 5.wav");//Brian Goal 3.wav");//Leave 8.wav");
  menuEnterBuffer.loadFromFile(   extraPath + "./sfx/Menu-Game/Leave 5.wav");
  menuExitBuffer.loadFromFile(    extraPath + "./sfx/Menu-Game/Leave 3.wav");
  errorBuffer.loadFromFile(       extraPath + "./sfx/Menu-Game/Error 3.wav");
  jumpBuffer[0].loadFromFile(     extraPath + "./sfx/Jump/Jump 2 Long.wav");
  jumpBuffer[1].loadFromFile(     extraPath + "./sfx/Jump/Jump 3 Long.wav");
  jumpBuffer[2].loadFromFile(     extraPath + "./sfx/Jump/Jump 9 Long.wav");
  jumpBuffer[3].loadFromFile(     extraPath + "./sfx/Jump/Jump 10 Long.wav");
  bumpBuffer[0].loadFromFile(     extraPath + "./sfx/Collision/Bump 6.wav");//Hit 1.wav");
  bumpBuffer[1].loadFromFile(     extraPath + "./sfx/Collision/Bump 4.wav");//Hit 4.wav");
  bumpBuffer[2].loadFromFile(     extraPath + "./sfx/Collision/Bump 1.wav");//Hit 6.wav"); // Original one true bump
  bumpBuffer[3].loadFromFile(     extraPath + "./sfx/Collision/Bump 5.wav");//Hit 14.wav");
  landBuffer[0].loadFromFile(     extraPath + "./sfx/Movement/Land 1.wav");
  landBuffer[1].loadFromFile(     extraPath + "./sfx/Movement/Land 2.wav");
  landBuffer[2].loadFromFile(     extraPath + "./sfx/Movement/Land 3.wav");
  landBuffer[3].loadFromFile(     extraPath + "./sfx/Movement/Land 4.wav");
  skidBuffer[0].loadFromFile(     extraPath + "./sfx/Movement/Skid 1.wav");
  skidBuffer[1].loadFromFile(     extraPath + "./sfx/Movement/Skid 2.wav");
  skidBuffer[2].loadFromFile(     extraPath + "./sfx/Movement/Skid 3.wav");
  skidBuffer[3].loadFromFile(     extraPath + "./sfx/Movement/Skid 4.wav");
  flipBuffer[0].loadFromFile(     extraPath + "./sfx/Movement/Flip 1.wav");
  flipBuffer[1].loadFromFile(     extraPath + "./sfx/Movement/Flip 2.wav");
  flipBuffer[2].loadFromFile(     extraPath + "./sfx/Movement/Flip 3.wav");
  flipBuffer[3].loadFromFile(     extraPath + "./sfx/Movement/Flip 4.wav");
  moveBuffer[0].loadFromFile(     extraPath + "./sfx/Movement/Move 1.ogg");
  moveBuffer[1].loadFromFile(     extraPath + "./sfx/Movement/Move 2.ogg");
  moveBuffer[2].loadFromFile(     extraPath + "./sfx/Movement/Move 3.ogg");
  moveBuffer[3].loadFromFile(     extraPath + "./sfx/Movement/Move 4.ogg");
  changeMenuBuffer.loadFromFile(  extraPath + "./sfx/Menu-Game/Menu 4.wav");
  changeOptionBuffer.loadFromFile(extraPath + "./sfx/Menu-Game/Menu 1.wav");
  focusCameraBuffer.loadFromFile( extraPath + "./sfx/Movement/Turn 10.wav");
  turnCameraBuffer.loadFromFile(  extraPath + "./sfx/Movement/Turn 1.wav");
  failCameraBuffer.loadFromFile(  extraPath + "./sfx/Collision/Hit 13.wav");

  // And setup sound players
  testSound.setBuffer(testBuffer);
  exitSound.setBuffer(exitBuffer);
  glowSound.setBuffer(glowBuffer);
  menuEnterSound.setBuffer(menuEnterBuffer);
  menuExitSound.setBuffer(menuExitBuffer);
  errorSound.setBuffer(errorBuffer);
  changeMenuSound.setBuffer(changeMenuBuffer);
  changeOptionSound.setBuffer(changeOptionBuffer);
  focusCameraSound.setBuffer(focusCameraBuffer);
  turnCameraSound.setBuffer(turnCameraBuffer);
  failCameraSound.setBuffer(failCameraBuffer);
  for (int i=0; i<4; i++) {
    jumpSound[i].setBuffer(jumpBuffer[i]);
    bumpSound[i].setBuffer(bumpBuffer[i]);
    landSound[i].setBuffer(landBuffer[i]);
    skidSound[i].setBuffer(skidBuffer[i]);
    flipSound[i].setBuffer(flipBuffer[i]);
    moveSound[i].setBuffer(moveBuffer[i]);
  }
  resetVolumes();

  // And finally, start playing the looping movement sounds!
  for (int i=0; i<4; i++) {
    moveSound[i].play();
    moveSound[i].setLoop(true);
  }
}

// Set all volumes again, since base volume was changed.
void resetVolumes() {
  
  testSound.setVolume(100*baseVolume/100.0);
  exitSound.setVolume(100*baseVolume/100.0);
  glowSound.setVolume(100*baseVolume/100.0);
  menuEnterSound.setVolume(100*baseVolume/100.0);
  menuExitSound.setVolume(100*baseVolume/100.0);
  errorSound.setVolume(100*baseVolume/100.0);
  changeMenuSound.setVolume(100*baseVolume/100.0);
  changeOptionSound.setVolume(100*baseVolume/100.0);
  focusCameraSound.setVolume(100*baseVolume/100.0);
  turnCameraSound.setVolume(100*baseVolume/100.0);
  failCameraSound.setVolume(100*baseVolume/100.0);

  for (int i=0; i<4; i++) {
    bumpSound[i].setVolume(75*baseVolume/100.0);
    landSound[i].setVolume(25*baseVolume/100.0);
    // These have volumes dependent on when they are called
    skidSound[i].setVolume(0*baseVolume/100.0);
    flipSound[i].setVolume(0*baseVolume/100.0);
    moveSound[i].setVolume(0*baseVolume/100.0);
  }
  jumpSound[0].setVolume(100*baseVolume/100.0);
  jumpSound[1].setVolume(100*baseVolume/100.0);
  jumpSound[2].setVolume( 65*baseVolume/100.0);
  jumpSound[3].setVolume( 65*baseVolume/100.0);
}

// Main loop for sound effects, called once per cycle
void sfxLoop() {

  // Unique changes and sounds for all players
  for (int i=0; i<cubiorCount; i++) {
    // Adjust volume if visibility changed
    if (getLastPlayerVisible(i) != sfxLastPlayerVisible[i]) {
      sfxLastPlayerVisible[i] = getLastPlayerVisible(i);
      // Newly visible
      if (sfxLastPlayerVisible[i]) {
        jumpVolumeMultiplier[i] = 100;
        bumpSound[i].setVolume(100*baseVolume/100.0);
        //landSound[i].setVolume(landVolume[i]*baseVolume/100.0);
        //skidSound[i].setVolume(100*baseVolume/100.0);
        /*if (i>=2) {
          jumpSound[i].setVolume(65);
          bumpSound[i].setVolume(100);
        } else {
          jumpSound[i].setVolume(100);
          bumpSound[i].setVolume(80);
        }*/
      } else {
        // Newly hidden, half volume
        jumpVolumeMultiplier[i] = 40;
        bumpSound[i].setVolume(50*baseVolume/100.0);
        //landSound[i].setVolume(0.5*landVolume[i]*baseVolume/100.0);
        //skidSound[i].setVolume(50*baseVolume/100.0);
        /*if (i>=2) {
          jumpSound[i].setVolume(21);
          bumpSound[i].setVolume(50);
        } else {
          jumpSound[i].setVolume(33);
          bumpSound[i].setVolume(40);
        }*/
      }
    }
    // Jumping SFX
    if (getCubiorJustJumped(i)) {
      jumpSound[i].play();
    }
    // Gets halfway there just by being >0 momentumY
    // but then must push up towards full momentum to get full volume
    jumpSound[i].setVolume(
      (50+jumpVolumeMultiplier[i]*getPlayer(i)->getMomentumY())
      /(getPlayer(i)->getMaxJump()*5)*baseVolume/100.0
    );
    // Bumping SFX
    if (getCubiorJustBumped(i)) {
      bumpSound[i].play();
    }
    // Landing SFX
    int newLandVol = getCubiorJustLanded(i);
    if (newLandVol > 0) {
      landVolume[i] = newLandVol;
      if (sfxLastPlayerVisible[i]) {
        landSound[i].setVolume(landVolume[i]*baseVolume/100.0);
      } else {
        landSound[i].setVolume(landVolume[i]*0.5*baseVolume/100.0);
      }
      landSound[i].play();
    }
    // Skidding SFX
    int newSkidVol = getCubiorJustSkidded(i);
    if (newSkidVol > 0) {
      skidVolume[i] = newSkidVol;
      if (sfxLastPlayerVisible[i]) {
        skidSound[i].setVolume(skidVolume[i]*baseVolume/100.0);
      } else {
        skidSound[i].setVolume(skidVolume[i]*0.5*baseVolume/100.0);
      }
      skidSound[i].play();
    }
    // Flipping SFX
    int newFlipVol = getCubiorJustFlipped(i);
    if (newFlipVol > 0) {
      flipVolume[i] = newFlipVol;
      if (sfxLastPlayerVisible[i]) {
        flipSound[i].setVolume(flipVolume[i]*baseVolume/100.0);
      } else {
        flipSound[i].setVolume(flipVolume[i]*0.5*baseVolume/100.0);
      }
      flipSound[i].play();
    }
    // Moving SFX
    int newMoveVol = getCubiorJustMoved(i);
    if (newMoveVol != moveVolume[i]) {
      moveVolume[i] = newMoveVol;
      if (sfxLastPlayerVisible[i]) {
        moveSound[i].setVolume(moveVolume[i]*baseVolume/100.0);
      } else {
        moveSound[i].setVolume(moveVolume[i]*0.5*baseVolume/100.0);
      }
    }
  }
  // Win Level
  if (getJustExited()) { 
    //exitSound.play(); 
    glowSound.play(); 
    setJustExited(false);
  }
  // Open Pause
  if (getJustPaused()) {
    menuEnterSound.play();
    setJustPaused(false);
  }
  // Close Pause
  if (getJustUnpaused()){
    menuExitSound.play(); 
    setJustUnpaused(false);
  }
  // Change Menu
  if (getJustChangedMenu()) {
    changeMenuSound.play();
    setJustChangedMenu(false);
  }
  // Change Menu
  if (getJustChangedOption()) {
    changeOptionSound.play();
    setJustChangedOption(false);
  }
  // Error Occurred
  if (getJustCausedError()) {
    errorSound.play();
    setJustCausedError(false);
  }
  // Camera Focused
  if (getJustFocusedCamera()) {
    focusCameraSound.play();
    setJustFocusedCamera(false);
  }
  // Camera Turned
  if (getJustTurnedCamera()) {
    turnCameraSound.play();
    setJustTurnedCamera(false);
  }
  // Camera Turn Failed
  if (getJustFailedCamera()) {
    failCameraSound.play();
    setJustFailedCamera(false);
  }
}

// For a simpler menu setting, cycle through one of three musical volumes
void cycleSoundVolume() {
  // Cycle up and around from min to max
  if (baseVolume == lowVolumeSfx) {
    baseVolume = medVolumeSfx;
  } else if (baseVolume == medVolumeSfx) {
    baseVolume = highVolumeSfx;
  } else if (baseVolume == highVolumeSfx) {
    baseVolume = offVolumeSfx;
  } else if (baseVolume == offVolumeSfx) {
    baseVolume = lowVolumeSfx;
  } else {
    // Not aligned to one of three presets? Make default.
    baseVolume = medVolumeSfx;
  }
  // Now that baseVolume is different, reset all SFX volumes!
  resetVolumes();
}

int getSoundVolume() {
  return baseVolume;
}

int getSoundVolumeNum() {
  if (baseVolume == offVolumeSfx) {
    return 0;
  } else if (baseVolume == lowVolumeSfx) {
    return 1;
  } else if (baseVolume == highVolumeSfx) {
    return 3;
  } else {
    // Default or anything else? Return Med
    return 2;
  }
}