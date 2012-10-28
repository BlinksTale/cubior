/*
 * image.h
 * by Brian Handy
 * 10/24/12
 * header for 2d (png) images for HUD/Menu/etc
 */
#ifndef IMAGE
#define IMAGE

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "lodepng.h" // for 2D images, all PNG based for transparency
  // lodePNG copyright (c) 2005-2012 Lode Vandevenne

class Image {
  
  protected:
    // Texture related things
    // based on stuff
    // from http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures
    std::vector<unsigned char> texture;
    unsigned int width, height;
    size_t u2, v2;
    double u3, v3;
    std::vector<unsigned char> imageItself;

public:
    Image();
    Image(const char*,float);
    void Image::draw(int,int,float,float);
};

#endif