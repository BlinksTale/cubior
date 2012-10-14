/*
 * cubeShape.cpp
 * by Brian Handy
 * 2/2/12
 * 3d cube model
 */
#include "cubeShape.h"
#include "flatRender.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream> // for cout
#include <algorithm> // for std::copy for colors
#include <stdio.h> // for pauseText
#include <time.h> // for printing timestamps
//#include <sys/time.h> // for linux time

// Copied from songho.ca example on vertex arrays
// http://www.songho.ca/opengl/gl_vertexarray.html
/*GLfloat vertices[] = {  0.5, 0.5, 0.5,  -0.5, 0.5, 0.5,  -0.5,-0.5, 0.5,   0.5,-0.5, 0.5,   // v0,v1,v2,v3 (front)
                        0.5, 0.5, 0.5,   0.5,-0.5, 0.5,   0.5,-0.5,-0.5,   0.5, 0.5,-0.5,   // v0,v3,v4,v5 (right)
                        0.5, 0.5, 0.5,   0.5, 0.5,-0.5,  -0.5, 0.5,-0.5,  -0.5, 0.5, 0.5,   // v0,v5,v6,v1 (top)
                       -0.5, 0.5, 0.5,  -0.5, 0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5,-0.5, 0.5,   // v1,v6,v7,v2 (left)
                       -0.5,-0.5,-0.5,   0.5,-0.5,-0.5,   0.5,-0.5, 0.5,  -0.5,-0.5, 0.5,   // v7,v4,v3,v2 (bottom)
                        0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5, 0.5,-0.5,   0.5, 0.5,-0.5 }; // v4,v7,v6,v5 (back)

*/
GLfloat vertices[] = {  0.5, 0.5, 0.5, // 0 - front upper left
                       -0.5, 0.5, 0.5, // 1 - front upper right
                        0.5,-0.5, 0.5, // 2 - front lower left
                       -0.5,-0.5, 0.5, // 3 - front lower right
                        0.5, 0.5,-0.5, // 4 - rear upper left
                       -0.5, 0.5,-0.5, // 5 - rear upper right
                        0.5,-0.5,-0.5, // 6 - rear lower left
                       -0.5,-0.5,-0.5  // 7 - rear lower right
                     };

// color array, modified from songho.ca code
/*GLfloat colors[]    = { 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)
                        0, 0, 1,   0, 0, 0,   0, 1, 0,   0, 1, 1 }; // v4,v7,v6,v5 (back)
*/
// Also directly lifted from songho.ca. Working with this on my own, but I figure
// it's better for now at least since I'm already using the tutorial's order
// for verticies.
// index array of vertex array for glDrawElements() & glDrawRangeElement()
GLubyte indices[]  = { // counterclockwise draws forward
                       // 0 left, 1 right, 2 top, 3 bot, 4 front, 5 rear
                       // Leftv
                       6, 4, 2, // left lower rear
                       2, 4, 0, // left upper front
                       // Right
                       1, 5, 3, // right upper front
                       7, 3, 5, // right lower rear
                       // Top
                       4, 1, 0, // upper front left
                       4, 5, 1, // upper rear right
                       // Bottom
                       3, 7, 2, // lower front right
                       6, 2, 7, // lower rear left
                       // Front
                       0, 1, 2, // front upper left  
                       2, 1, 3, // front lower right 
                       // Back
                       4, 6, 5, // rear upper left
                       7, 5, 6 // rear lower right
                     }; 

GLubyte CubeShape::getIndex(int i) {
  return indices[i];
}

GLfloat CubeShape::getVertex(int i) {
  return myVertices[i];
}

GLfloat CubeShape::getColor(int i) {
  return myColors[i];
}

void CubeShape::initVisuals(float nR, float nG, float nB, float nR2, float nG2, float nB2, float colorDarkness, bool alt, bool mid) {
  midFloor = mid;
  alternatingSpot = alt;
  altDark = alternatingSpot * 0.125;
  // Nothing here yet! Colors are what distinguish Cubiors from Cubes atm
  r1 = nR - altDark;
  g1 = nG - altDark;
  b1 = nB - altDark;
  r2 = nR - colorDarkness - altDark;
  g2 = nG - colorDarkness - altDark;
  b2 = nB - colorDarkness - altDark;
  r3 = nR2 - altDark;
  g3 = nG2 - altDark;
  b3 = nB2 - altDark;

  // Did not want to have to make an array every draw,
  // but otherwise all the arrays meld into one,
  // and all turn the last color submitted -
  // red for the goal. So here we are!
  GLfloat newColors[] = { r1, g1, b1, // front top left
                        r1, g1, b1, // front top right
                        r2, g2, b2, // front bottom left
                        r2, g2, b2, // front bottom right
                        r1, g1, b1, // back top left
                        r1, g1, b1, // back top right
                        r2, g2, b2, // back bottom left
                        r2, g2, b2  // back bottom right
                      }; 

  for (int i=0; i<24; i++) {
    myColors[i] = newColors[i];
  }
  
  // default shadow status is no
  defaultHasShadow = false;
  shadowState = false;
  hasShadowResult = (shadowState || defaultHasShadow);

  // extra wall culling
  directionalCulling = true;
  leftCam = false;
  aboveCam = false;
  behindCam = false;
   
  /*std::cout << "rbg1: " << r1 << ", " << g1 << ", " << b1 << std::endl;
  std::cout << "rbg2: " << r2 << ", " << g2 << ", " << b2 << std::endl;
  std::cout << "rbg3: " << r3 << ", " << g3 << ", " << b3 << std::endl << std::endl;
  
  for (int i=0; i<24; i++) {
    std::cout << "newColors[" << i << "] is " << newColors[i] << std::endl;
    colors[i] = newColors[i];
    std::cout << "colors[" << i << "] is " << colors[i] << std::endl;
  }*/
  //std::copy(newColors, newColors + 3*8, colors);
        /*case 2: // top        
          glColor3f(r3,g3,b3);
          break;
        case 5: // back         
          glColor3f(r2-0.2,g2-0.2,b2-0.2);
          break;
        default:    
          glColor3f(r1,g1,b1); // for the top verticies*/
  //for (int i=0; i<24; i++) {
  //}
  for (int i=0; i<24; i++) {
    myVertices[i] = vertices[i];
  }
}

void CubeShape::setRelationToCam(bool a, bool b, bool c) {
  leftCam = a;
  aboveCam = b;
  behindCam = c;
}

void CubeShape::updateVisuals() {
  // This handles colors and face smile for Cubiors, but nothing for Cubes yet
}

void CubeShape::draw() {
  
  // specify pointer to vertex array
  glColorPointer(3, GL_FLOAT, 0, myColors);
  glVertexPointer(3, GL_FLOAT, 0, myVertices);
  
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
}

// This lets you skip transforms for unmoving objects later, and scaling!
void CubeShape::permanentPosition(int x, int y, int z) {
  for (int i=0; i<8; i++) {

    //std::cout << " Just merged " << vertices[i*3+0]*100 << ", " << vertices[i*3+1]*100 << ", " << vertices[i*3+2]*100;
    //std::cout << " with " << x << ", " << y << ", " << z;
    myVertices[i*3+0] = vertices[i*3+0]*100+x;//(vertices[i*3+0] + x);
    myVertices[i*3+1] = vertices[i*3+1]*100+y;//(vertices[i*3+1] + y);
    myVertices[i*3+2] = vertices[i*3+2]*100+z;//(vertices[i*3+2] + z);
    //std::cout << " to get " << myVertices[i*3+0] << ", " << myVertices[i*3+1] << ", " << myVertices[i*3+2] << std::endl;
  }
}

// OLD Draw faces TECHNIQUE KEPT AROUND JUST IN CASE
  /*for (int i=0; i<6; i++) {
    if (
         (!useNeighbors) // either don't use neighbors, and draw everything!
         || // or...
         ( // if you do use neighbors, 
           (!neighbors[i]) // If you don't have THIS neighbor
           && // and...
           ( // either
             (!directionalCulling) // you don't use directional culling
             || // or...
             ( // You do, and...
               (i!=0 ||   !leftCam) && (i!=1 || leftCam)   // camera can't see left/right of what's to the left/right of it
            && (i!=2 ||  !aboveCam) && (i!=3 || aboveCam)  // camera can't see top/bot of what's above/below it
            && (i!=4 || !behindCam) && (i!=5 || behindCam) // camera can't see back/front of what's in front of/behind it
             // Get rid of faces on other side from you
             // Each dimension recovers about 3fps
             )
           )
         )
       ) { // So draw that face!
      
      // Top is special
      if (i==2) {
        // Disable array colors,
        glDisableClientState(GL_COLOR_ARRAY);
        // use a special color
        glColor3f(r3,g3,b3);
      }

      // Trying new drawElements approach
      // used to be DrawRangeElements, but Windows didn't like that. Using drawElements now... is this more inefficient?
      //glDrawRangeElements(GL_TRIANGLES, 0, 3, 6, GL_UNSIGNED_BYTE, indices+6*i);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices+6*i);

      // Top has special cleanup too
      if (i==2) {
        // and re-enable array colors
        glEnableClientState(GL_COLOR_ARRAY);
      }
    }
  }*/
  
// Grabs your rgb1 colors and makes a dark version of yourself
// Perfect for walking behind walls, but player identification
void CubeShape::drawSilhouette() {
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_BLEND );
  glPushMatrix();
  //glScalef(1.1,1.1,1.1);
  //glScalef(0.99,0.99,0.99);
  // These code blocks modified from work on songho.ca
  // activate and specify pointer to vertex array
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices);

  // draw first half, range is 6 - 0 + 1 = 7 vertices used
  for (int i=0; i<6; i++) {
    if (!useNeighbors || !neighbors[i]) { // 0 left, 1 right, 2 top, 3 bot, 4 front, 5 rear
      glColor4f(r1*0.25+0.125,g1*0.25+0.125,b1*0.25+0.125,0.5);
      // 0 to 3 means we only have four vertices used for each face
      // 6 is the total points we create though from those four.
      // indices+6*i is where we are looking, which face in indices
      
      // used to be DrawRangeElements, but Windows didn't like that. Using drawElements now... is this more inefficient?
      //??glDrawRangeElements(GL_TRIANGLES, 0, 3, 6, GL_UNSIGNED_BYTE, indices+6*i);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices+6*i);
    }
  }

  // deactivate vertex arrays after drawing
  glDisableClientState(GL_VERTEX_ARRAY);
  glPopMatrix();
  glDisable( GL_BLEND );
}

// Cell-shading-like outline!
void CubeShape::drawOutline() {
  glDisable(GL_DEPTH_TEST); // disable depth test for "shadow cubior"
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_BLEND );
  drawOutlineVolume();
  glDisable( GL_BLEND );
  glEnable(GL_DEPTH_TEST); // Then return to normal stuff
}

void CubeShape::drawOutlineVolume() {
  glPushMatrix();
  glScalef(1.1,1.1,1.1);
  // These code blocks modified from work on songho.ca
  // activate and specify pointer to vertex array
  //glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices);

  // draw first half, range is 6 - 0 + 1 = 7 vertices used
  for (int i=0; i<6; i++) {
    if (!useNeighbors || !neighbors[i]) { // 0 left, 1 right, 2 top, 3 bot, 4 front, 5 rear
      glColor4f(0,0,0,1);
      // used to be DrawRangeElements, but Windows didn't like that. Using drawElements now... is this more inefficient?
      //??glDrawRangeElements(GL_TRIANGLES, 0, 3, 6, GL_UNSIGNED_BYTE, indices+6*i);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices+6*i);
    }
  }

  // deactivate vertex arrays after drawing
  //glDisableClientState(GL_VERTEX_ARRAY);
  glPopMatrix();
}

void CubeShape::setNeighbors(bool newNeighbors[6]) { 
  for (int i=0; i< 6; i++) {
    neighbors[i] = newNeighbors[i];
  }
  //shadowState = !neighbors[2]; // It's either 2 or 3, those are the y ones
}

// using a hackey shadow volumes technique, create our
// own tall cube beneath main cube shadow volumes.
void CubeShape::drawShadow() {
  if (hasShadow()) {
    glCullFace(GL_FRONT);
    glStencilFunc(GL_ALWAYS, 0x0, 0xff);
    glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
    drawShadowVolume();

    glCullFace(GL_BACK);
    glStencilFunc(GL_ALWAYS, 0x0, 0xff);
    glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
    drawShadowVolume();
  }
}


// Shadow volume shape, represents where shadow will fall
void CubeShape::drawShadowVolume() {
  // Actual shadow volume stuff
  glPushMatrix();
  glScalef(1.0,100.0,1.0);
  glTranslatef(0.0,-0.505,0.0);

  // If no sides to connect to... make a bit smaller, to avoid overlapping edges
  if (neighbors[0] && neighbors[1] && neighbors[4] && neighbors[5]) {
    glScalef(1.01,1.0,1.01);
  } else if (neighbors[0] && neighbors[1]) {
    glScalef(1.01,1.0,0.999);
  } else if (neighbors[4] && neighbors[5]) {
    glScalef(0.999,1.0,1.01);
  } else {
    glScalef(0.999,1.0,0.999);
  }
  // These code blocks modified from work on songho.ca
  // activate and specify pointer to vertex array
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices);

  glColor4f(0.125,0.125,0.125,0.5);
  // draw first half, range is 6 - 0 + 1 = 7 vertices used
  for (int i=0; i<6; i++) {
    //if (!useNeighbors || !neighbors[i]) { // 0 left, 1 right, 2 top, 3 bot, 4 front, 5 rear
      // 0 to 3 means we only have four vertices used for each face
      // 6 is the total points we create though from those four.
      // indices+6*i is where we are looking, which face in indices

      // used to be DrawRangeElements, but Windows didn't like that. Using drawElements now... is this more inefficient?
      //??glDrawRangeElements(GL_TRIANGLES, 0, 3, 6, GL_UNSIGNED_BYTE, indices+6*i);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices+6*i);
    //}
  }
  // deactivate vertex arrays after drawing
  glDisableClientState(GL_VERTEX_ARRAY);
  glPopMatrix();
}

// Only have a shadow if no lower neighbor
bool CubeShape::hasShadow() {
  return hasShadowResult;
}

bool CubeShape::hasFace(int i) {
  return !neighbors[i];
}

bool CubeShape::hasVisibleFace() {
  bool result;
  for (int i=0; i<6; i++) {
    result = result || (!neighbors[i]);
  }
  return result;
}

void CubeShape::setShadow(bool b) {
  shadowState = b;
  hasShadowResult = (shadowState || defaultHasShadow);
}