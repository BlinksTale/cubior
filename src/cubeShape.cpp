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

float sV = 0.45; // shadow vertex
float nV = 0.55; // neighbor vertex
float mV = 100.0; // max vertex

GLfloat shadowVerts[] = {
                        sV, sV, sV, // 0 - front upper left
                       -sV, sV, sV, // 1 - front upper right
                        sV,-mV, sV, // 2 - front lower left
                       -sV,-mV, sV, // 3 - front lower right
                        sV, sV,-sV, // 4 - rear upper left
                       -sV, sV,-sV, // 5 - rear upper right
                        sV,-mV,-sV, // 6 - rear lower left
                       -sV,-mV,-sV  // 7 - rear lower right
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

GLfloat CubeShape::getTopColor(int i) {
  return topColors[i];
}

GLfloat CubeShape::getShadowVertex(int i) {
  return shadowVerts[i]*100 + ((i%3==0)? permanentX : (i%3==1)? permanentY : permanentZ);
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

  // Can just make a new array and transfer contents into myColors
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
  
  // And assign top colors
  GLfloat newTopColors[] = { r3, g3, b3, r3, g3, b3, r3, g3, b3, r3, g3, b3 }; // all top corners
  for (int i=0; i<12; i++) {
    topColors[i] = newTopColors[i];
  }

  // default shadow status is no
  shadowState = false;

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
    //myShadowVertices[i] = vertices[i];
  }
}

void CubeShape::setRelationToCam(bool a, bool b, bool c) {
  leftCam = a;
  aboveCam = b;
  behindCam = c;
}

void CubeShape::updateVisuals() {
  // Make sure colors are up to date!

  // Can just make a new array and transfer contents into myColors
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
  
  // And assign top colors
  GLfloat newTopColors[] = { r3, g3, b3, r3, g3, b3, r3, g3, b3, r3, g3, b3 }; // all top corners
  for (int i=0; i<12; i++) {
    topColors[i] = newTopColors[i];
  }

}

void CubeShape::draw() {
  
  // specify pointer to vertex array
  glColorPointer(3, GL_FLOAT, 0, myColors);
  glVertexPointer(3, GL_FLOAT, 0, myVertices);
  
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
}

// This lets you skip transforms for unmoving objects later, and scaling!
void CubeShape::permanentPosition(int x, int y, int z) {
  permanentX = x;
  permanentY = y;
  permanentZ = z;
  for (int i=0; i<8; i++) {

    //std::cout << " Just merged " << vertices[i*3+0]*100 << ", " << vertices[i*3+1]*100 << ", " << vertices[i*3+2]*100;
    //std::cout << " with " << x << ", " << y << ", " << z;
    myVertices[i*3+0] = vertices[i*3+0]*100+x;
    myVertices[i*3+1] = vertices[i*3+1]*100+y;
    myVertices[i*3+2] = vertices[i*3+2]*100+z;
    //myShadowVertices[i*3+0] = shadowVerts[i*3+0]*100+x;
    //myShadowVertices[i*3+1] = shadowVerts[i*3+1]*100+y;
    //myShadowVertices[i*3+2] = shadowVerts[i*3+2]*100+z;
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

void CubeShape::setNeighborObjects(CubeObj* newNeighbors[6]) {
  for (int i=0; i< 6; i++) {
    neighborObjects[i] = newNeighbors[i];
  }
}

// using a hackey shadow volumes technique, create our
// own tall cube beneath main cube shadow volumes.
void CubeShape::drawShadow() {
  if (shadowState) {
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
  return shadowState;
}

bool CubeShape::hasFace(int i) {
  return !neighbors[i];
}

bool CubeShape::hasVisibleFace() {
  bool result = false;
  for (int i=0; i<6; i++) {
    result = result || (!neighbors[i]);
  }
  return result;
}

void CubeShape::setShadow(bool b) {
  shadowState = b;

  if (shadowState) {
    // So if you set your neighbors, update shadows to reflect this!
    // if no neighbor, absolutely must reset shadowVerts to sV, or it doesn't work
    // Can't explain why it doesn't worked, but confirmed to not work otherwise.
    // Not a big loss though, just a hair longer on loading... so, nothing
    // Neighbors: 0 left, 1 right, 2 top, 3 bot, 4 front, 5 rear
    if (neighbors[0] == 1) { // left
      shadowVerts[0*3+0] = nV;
      shadowVerts[2*3+0] = nV;
      shadowVerts[4*3+0] = nV;
      shadowVerts[6*3+0] = nV;
    } else { // shadow verts instead of neighbor verts
      shadowVerts[0*3+0] = sV;
      shadowVerts[2*3+0] = sV;
      shadowVerts[4*3+0] = sV;
      shadowVerts[6*3+0] = sV;
    }
    if (neighbors[1] == 1) { // right
      shadowVerts[1*3+0] =-nV;
      shadowVerts[3*3+0] =-nV;
      shadowVerts[5*3+0] =-nV;
      shadowVerts[7*3+0] =-nV;
    } else { // shadow verts instead of neighbor verts
      shadowVerts[1*3+0] =-sV;
      shadowVerts[3*3+0] =-sV;
      shadowVerts[5*3+0] =-sV;
      shadowVerts[7*3+0] =-sV;
    }
    if (neighbors[4]) { // front
      shadowVerts[0*3+2] = nV;
      shadowVerts[1*3+2] = nV;
      shadowVerts[2*3+2] = nV;
      shadowVerts[3*3+2] = nV;
    } else { // shadow verts instead of neighbor verts
      shadowVerts[0*3+2] = sV;
      shadowVerts[1*3+2] = sV;
      shadowVerts[2*3+2] = sV;
      shadowVerts[3*3+2] = sV;
    }
    if (neighbors[5]) { // rear
      shadowVerts[4*3+2] =-nV;
      shadowVerts[5*3+2] =-nV;
      shadowVerts[6*3+2] =-nV;
      shadowVerts[7*3+2] =-nV;
    } else { // shadow verts instead of neighbor verts
      shadowVerts[4*3+2] =-sV;
      shadowVerts[5*3+2] =-sV;
      shadowVerts[6*3+2] =-sV;
      shadowVerts[7*3+2] =-sV;
    }
  }
}

// Check neighbors for like colors. Delete neighbor's indices and extend self if case found
void CubeShape::removeDuplicateNeighbors() {
  // If we found a duplicate (neighbor to the right AND same color)
  // WIP: Assume same color
  CubeObj* currentObj = neighborObjects[0];
  int width = 1; // where we start, one cube wide
  if (neighbors[0]) {
    // Keep going after right side cubes until we hit one that doesn't match us 
    while (true) {//width < currentObj->getAlternatingSpotSize()/100) { // for right direction
      if ( currentObj->getMaterial() == material
        && currentObj->getAlternatingSpot() == alternatingSpot
        && atLeastAllNeighborsOf(currentObj->getVisibleNeighbors())
        && !currentObj->getDuplicateNeighbor()) {
        // Extend self since duplicate found
        for (int i=0; i<24; i++) {
          //vertices[i*3] = 0.0;
          if (i%6 == 0) {
            myVertices[i] += 100.0; // extend one to the right
          }
        }
        // And hide the original object
        currentObj->setDuplicateNeighbor(true);

        // Must be a bit wider since we added a cube!
        width++;

        // Now since this was a success, and further neighbors
        // of similar color/material/neighbors were found,
        // grab this object's right side neighbor and do it again!
        currentObj = currentObj->getVisibleNeighborObjects()[0];
      } else {
        // No new neighbor of same color/material/darkness/neighborhood, get out!
        break;
      }
    }
  }
  // Next, check backwards!
  CubeObj* oldCurrentObj = neighborObjects[4]; // use for removal process
  currentObj = oldCurrentObj; // use for search process
  int depth = 1; // start over with width again
  if (neighbors[4]) {
    // Keep going after back side cubes until we hit one that doesn't match us 
    // unless no neighbor there or neighbor is already a duplicate
    while (oldCurrentObj->getVisibleNeighbors()[4] && !(oldCurrentObj->getVisibleNeighborObjects()[4]->getDuplicateNeighbor())) {//depth < currentObj->getAlternatingSpotSize()/100) { // for backwards direction
      // Make sure we're still only seeing the same material/neighbors/etc
      bool stillTheSame = true;
      currentObj = oldCurrentObj; // use for search process
      
      // Then make sure we can go right the same dist as our width!
      for (int i=0; i<width; i++) {
        if (stillTheSame) {
          stillTheSame = stillTheSame
            && currentObj->getMaterial() == material
            && currentObj->getAlternatingSpot() == alternatingSpot
            && atLeastAllNeighborsOf(currentObj->getVisibleNeighbors())
            && !currentObj->getDuplicateNeighbor();
        }
        if (stillTheSame && currentObj->getVisibleNeighbors()[0]) {
          // Great! Now move along and make sure it's true for the next one too
          currentObj = currentObj->getVisibleNeighborObjects()[0];
        } else {
          // Something was different or didn't exist! Oh well
          stillTheSame = false;
          break;
        }
      }

      // So if nothing was different, act on it!
      if (stillTheSame) {
        // First, we'll be starting with the oldCurrentObj as the currentObj
        // (search is done, removal begins)
        currentObj = oldCurrentObj;
        // Extend self since entire duplicate row found
        for (int i=0; i<24; i++) {
          if (i%3 == 2 && i < 12) {
            myVertices[i] += 100.0; // extend one to the back
          }
        }
        // And hide all the original objects
        for (int i=0; i<width; i++) {
          currentObj->setDuplicateNeighbor(true);
          // Great! Now move backwards and make sure to remove the last one too
          currentObj = currentObj->getVisibleNeighborObjects()[0];
        }

        // Must be a bit deeper since we added a row of cubes!
        depth++;

        // Now since this was a success, and further neighbors
        // of similar color/material/neighbors were found,
        // grab the old object's rear side neighbor and do it again!
        oldCurrentObj = oldCurrentObj->getVisibleNeighborObjects()[4];
      } else {
        // No new neighbor of same color/material/darkness/neighborhood, get out!
        break;
      }
    }
  }
}

void CubeShape::setMaterial(int i) {
  material = i;
}

int CubeShape::getMaterial() {
  return material;
}

// Compare all my neighbors to another set
bool CubeShape::sameNeighborsAs(bool newNeighbors[6]) {
  bool result = true;
  for (int i=0; i<6; i++) {
    result = result && (newNeighbors[i] == neighbors[i]);
  }
  return result;
}

// Compare all my neighbors to another set, but only need at least what it has
bool CubeShape::atLeastAllNeighborsOf(bool newNeighbors[6]) {
  bool result = true;
  for (int i=0; i<6; i++) {
    // so there is a new neighbor there (empty face) OR no old neighbor there (solid face)
    result = result && (newNeighbors[i] || !neighbors[i]);
  }
  return result;
}