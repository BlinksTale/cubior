/*
 * Cubior
 * by Brian Handy
 * 1/13/12
 * Main class for 3d platformer
 */

#include <iostream>
#include "visuals.h"
#include "gameplay.h"

#include "ResourcePath.hpp" // to load (in XCode for Mac) from app's resource folder using the SFML file (combined with ResourcePath.mm)

using namespace std;

bool networkingOption = false;

//#include <SFML/Graphics.hpp>


// Gets called as often as possible
/*void tick() {
  //gameplayLoop();
  //updateGraphics(x);
  //render();
}*/


/* JUST FOR NOW, COMING BACK AS SOON AS SFML TEST RUNS*/
int main(int argc, char** argv) {
    
    // Ask first for any other PC to connect to
    string boundAddress;
    
    if (networkingOption) {
        cout << "Would you like to play networked multiplayer? (y/n)" << endl;
        cin >> boundAddress;
        if (boundAddress.compare("y") == 0) {
            // Bind to a specific address
            cout << "What address would you like to bind to? (eg. 127.0.0.1)" << endl;
            cin >> boundAddress;

            // Shortcut for Home is "n"
            if (boundAddress.compare("n") == 0 || boundAddress.compare("y") == 0) {
            boundAddress = "127.0.0.1";
            }
        } else {
            boundAddress = "n";
        }
    } else {
      boundAddress = "n";
    }
    
    // FIXME: Override for the address check in gameplay
    // so that networking is made ready in time, even
    // if connection is not made immediately
    boundAddress = "127.0.0.1";

    // Create a test file for cubior to see where working dir is in xcode
    /*FILE *f = fopen("cubiorTestFile.txt","w+"); fprintf(f, "hello I am a cubior test file"); fclose(f);
    
    FILE * pFile;
    pFile = fopen ("myCubiorFile.txt","w");
    if (pFile!=NULL)
    {
        fputs ("fopen example",pFile);
        fclose (pFile);
    }*/
    
  // Extra path ensures Resources folder is accessed for .app on Mac
  std::string extraPath = ".";
  #ifdef __APPLE_CC__
    extraPath = resourcePath();
  #endif
  gameplayInit(extraPath + "/maps/cubiorMap0.cubior", boundAddress);
  initRender(argc, argv); // nothing gets called after this
  //tick();
    /**/
}
 

/*
LodePNG Examples

Copyright (c) 2005-2012 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

//Compile command for Linux:
//g++ lodepng.cpp example_opengl.cpp -lSDL -lGL -O3

/*
LodePNG OpenGL example. Decodes a PNG and shows it in OpenGL. PNG filename
should be given as a command line parameter.

It's written for the most basic old OpenGL version, and a correction for non
power of two textures had to be added.

Only very few lines on the sample are about loading the PNG. Most of the
sample lines show a way to render a texture in 2D in OpenGL.

No fancy 3D graphics are shown, it only shows the image statically. The sample
shows LodePNG can be used to load PNG images as textures in OpenGL.
*/
/*
#include "lodepng.h"

#include <iostream>

#ifdef __APPLE_CC__ 
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/gl.h>

int main(int argc, char *argv[])
{
  const char* filename = "./images/CubiorLogo720.png";//argv[1];

  // Load file and decode image.
  std::vector<unsigned char> image;
  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, filename);

  // If there's an error, display it.
  if(error != 0)
  {
    std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
    return 1;
  }
  
  // Here the PNG is loaded in "image". All the rest of the code is SDL and OpenGL stuff.

  int screenw = width;
  if(screenw > 1600) screenw = 1600;
  int screenh = height;
  if(screenh > 768) screenh = 768;

  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL);
    
  // setup & create window
  glutInitWindowPosition(0,0);
  glutInitWindowSize(screenw,screenh);
  glutCreateWindow("Cubior");

  // The official code for "Setting Your Raster Position to a Pixel Location" (i.e. set up a camera for 2D screen)
  glViewport(0, 0, screenw, screenh);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, screenw, screenh, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Make some OpenGL properties better for 2D and enable alpha channel.
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);

  // Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
  size_t u2 = 1; while(u2 < width) u2 *= 2;
  size_t v2 = 1; while(v2 < height) v2 *= 2;
  // Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
  double u3 = (double)width / u2;
  double v3 = (double)height / v2;

  // Make power of two version of the image.
  std::vector<unsigned char> image2(u2 * v2 * 4);
  for(size_t y = 0; y < height; y++)
  for(size_t x = 0; x < width; x++)
  for(size_t c = 0; c < 4; c++)
  {
    image2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
  }
  
  // Enable the texture for OpenGL.
  glEnable(GL_TEXTURE_2D);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);
  
  bool done = false;
  float textureX = 10;
  float textureY = 10;
  // Not sure why this was used here
  //glColor4ub(255, 255, 255, 255);
  // No difference when commented out
    

    while (true) {
      textureX += 0.1;
      glClearColor(0.5, 0, 0.5, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
      // Draw the texture on a quad, using u3 and v3 to correct non power of two texture size.
      glBegin(GL_QUADS);
        glTexCoord2d( 0,  0); glVertex2f(      textureX,        textureY);
        glTexCoord2d(u3,  0); glVertex2f(textureX+width,        textureY);
        glTexCoord2d(u3, v3); glVertex2f(textureX+width, textureY+height);
        glTexCoord2d( 0, v3); glVertex2f(      textureX, textureY+height);
      glEnd();
    
      glutSwapBuffers();
    }
}
*/