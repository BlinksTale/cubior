/*
 * image.cpp
 * by Brian Handy
 * 10/24/12
 * A 2D Graphic, PNG based, for menus
 */

#include "image.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "lodepng.h" // for 2D images, all PNG based for transparency
  // lodePNG copyright (c) 2005-2012 Lode Vandevenne

#include <iostream> // for couts
using namespace std;

// No file name? Hardly an image now is it.
Image::Image() {
  u2 = 0;
  u3 = 0;
  v2 = 0;
  v3 = 0;
}

// Constructor! Take a file name in, and how much a fraction it is of intended size
Image::Image(const char* name, float multiplier) {
  height = 0;
  width = 0;
  unsigned error1 = lodepng::decode(texture, width, height, name);
  cout << "Loading texture " << name << " of size " << width << ", " << height << endl;
  
  // Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
  u2 = 1; while(u2 < width) u2 *= 2;
  v2 = 1; while(v2 < height) v2 *= 2;
  // Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
  u3 = (double)width / u2;
  v3 = (double)height / v2;
  
  // Would like to move this out of draw if possible
  // Make power of two version of the image.
  std::vector<unsigned char> image2(u2 * v2 * 4);
  for(size_t y = 0; y < height; y++)
  for(size_t x = 0; x < width; x++)
  for(size_t c = 0; c < 4; c++)
  {
    image2[4 * u2 * y + 4 * x + c] = texture[4 * width * y + 4 * x + c];
  }

  imageItself = image2;

  height *= multiplier;
  width *= multiplier;
}

// Draw self at a location
void Image::draw(int x, int y, float aspect, float rotate) {
  
  glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageItself[0]);
  
  float currentTextureX = x;
  float currentTextureY = y;

  //glTranslatef(0.0,0.0,10.0);
  glPushMatrix();
    // Always keep in center of screen, regardless of size/resolution
    // And use aspect from earlier to do this, and 1600 as expected/base width
    if (aspect > 1.0) {
      glTranslatef(1600*aspect/2+currentTextureX, 0.0f+height/2+currentTextureY,0.0f);
    } else {
      glTranslatef(1600/2+currentTextureX,        0.0f+height/2+currentTextureY,0.0f);
    }
    glPushMatrix();
    glRotatef(rotate,0.0,0.0,1.0);
    // Draw the texture on a quad, using u3 and v3 to correct non power of two texture size.
    glBegin(GL_QUADS);
      glTexCoord2d( 0,  0); glVertex2f(0-width/2.0, 0-height/2.0);
      glTexCoord2d(u3,  0); glVertex2f(0+width/2.0, 0-height/2.0);
      glTexCoord2d(u3, v3); glVertex2f(0+width/2.0, 0+height/2.0);
      glTexCoord2d( 0, v3); glVertex2f(0-width/2.0, 0+height/2.0);
    glEnd();
  glPopMatrix();
  glPopMatrix();
}