/*
 * visuals.cpp
 * by Brian Handy
 * 1/19/12
 * Visuals for cubior
 */
#include <iostream>
#include "visuals.h"
#include "textRender.h"

int renderMode = 0;

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

void renderFlat() {
  std::cout << "Coming Soon.\n";
}

void renderDepth() {
  std::cout << "Coming a bit later.\n";
}

void renderWrong() {
  std::cout << "Brrrt! Choose a correct render mode!\n";
}
