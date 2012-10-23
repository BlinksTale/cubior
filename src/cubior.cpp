/*
 * Cubior
 * by Brian Handy
 * 1/13/12
 * Main class for 3d platformer
 */
#include <iostream>
#include "visuals.h"
#include "gameplay.h"
using namespace std;

#include <SFML/Graphics.hpp>

#include <iostream>




// Gets called as often as possible
/*void tick() {
  //gameplayLoop();
  //updateGraphics(x);
  //render();
}

int main(int argc, char** argv) {
  gameplayStart("./maps/cubiorMap0.cubior");
  initRender(argc, argv); // nothing gets called after this
  //tick();
  
    /*sf::Clock Clock;
    while (Clock.GetElapsedTime() < 15.f)
    {
		//sf::Event::Joystick::update(); // must be called once per cycle since no SFML window
        std::cout << Clock.GetElapsedTime() << std::endl;
		//std::cout << sf::Joystick::isConnected(0) << std::endl;
		float phil = (sf::Event::JoyMoveEvent()).Position;
        std::cout <<  phil << std::endl;
        sf::Sleep(0.5f);
    }*/

    //sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    //sf::CircleShape shape(100.f);
    //shape.setFillColor(sf::Color::Green);

	//sf::Clock Clock;
	
	/*
	AND THE JOYSTICKS AREEEEE:
	For me/now at least,
	Controller 1: Joystick 1 // wired
	Controller 2: Joystick 2 // wireless 1
	Controller 3: Joystick 0 // wireless 2
	Controller 4: Joystick 3 // wireless 3

	Axis:
    X: Left Stick X (-100 left to 100 right)
	Y: Left Stick Y (-100 up to 100 down)
	Z: L and R (99 L to -99 R)
	R: Right Y (-100 up)
	U: Right X (-100 left)
	V: -100 locked?
	PovX: Dpad Y up (100 up!!!! to 0 for down?) and D-pad X for sure (-92 left to 92 right)
	PovY: Dpad Y down (100 down?) and Dpad X too? (38 left to 70 right?)
    */
	/*cout << "PROGRAM INITIATED" << endl;
	while (true) {
		sf::Joystick::update();
		if (sf::Joystick::isButtonPressed(1,0)) {
			for (int i=0; i<8; i++) {
				cout << "Joystick  " << i << ": " << sf::Joystick::isConnected(i) << endl;
				cout << "stats for " << i << ": " << sf::Joystick::getAxisPosition(i, sf::Joystick::X) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::Y) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::Z) << ", " << sf::Joystick::getAxisPosition(i, sf::Joystick::R) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::U) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::V) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::PovX) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::PovY) << endl;
			}
			cout << "----" << endl;
		}
    }*/

   // return 0;
//}

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

#include "lodepng.h"

#include <iostream>

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/gl.h>

int main(int argc, char *argv[])
{/*
  if(argc < 2)
  {
    std::cout << "Please provide a filename." << std::endl;
    return 1;
  }*/
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

  /*if(glGetError() != GL_NO_ERROR)
  {
    std::cout << "Error initing GL" << std::endl;
    return 1;
  }*/
  
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
