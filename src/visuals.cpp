/*
 * visuals.cpp
 * by Brian Handy
 * 1/19/12
 * Visuals for cubior
 */
#include <iostream>
#include "visuals.h"
#include "textRender.h"
#include "flatRender.h"

int renderMode = 2;

void initRender(int argc, char** argv) {
  switch(renderMode){
    case 2:
      initFlat(argc, argv); // nothing gets called after this
      break;
    default:
      break;
  }  
}

void updateGraphics(int x) {
  switch(renderMode){
    case 2:
      updateFlat();
      break;
    default:
      break;
  }  
}

void render() {
  switch(renderMode){
    case 0:
      renderText();
      break;
    case 1:
      renderASCII();
      break;
    case 2:
      renderFlat();
      break;
    case 3:
      renderDepth();
      break;
    default:
      renderWrong();
      break;
  }  
}

// RenderText can be found in textRender.cpp

void renderASCII() {
  std::cout << "[^_^]\n";
}

// RenderFlat can be found in flatRender.cpp

void renderDepth() {
  std::cout << "Coming a bit later.\n";
}

void renderWrong() {
  std::cout << "Brrrt! Choose a correct render mode!\n";
}
