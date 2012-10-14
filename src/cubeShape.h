/*
 * cubeShape.h
 * by Brian Handy
 * 2/2/12
 * header for 3d model of cube
 */
#ifndef CUBESHAPE
#define CUBESHAPE

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class CubeShape {
  
  protected:
    bool alternatingSpot, midFloor; // for tile color and sides drawn
    bool shadowState; // default is true later for player/goal
    bool directionalCulling, aboveCam, behindCam, leftCam; // extra face culling stuff
    bool neighbors[6]; // points to neighbors array of cubeObj
    int permanentX, permanentY, permanentZ;
    float altDark;
    float r1,g1,b1,r2,b2,g2,r3,g3,b3;
    const static bool useNeighbors = true;
    GLfloat myVertices[24], myColors[24], topColors[12];//, myShadowVertices[24];
    //GLfloat myShadowVertices[24];
  public:
    virtual GLubyte getIndex(int);
    virtual GLfloat getVertex(int);
    virtual GLfloat getColor(int);
    virtual GLfloat getTopColor(int);
    virtual GLfloat getShadowVertex(int);
    virtual void initVisuals(float,float,float,float,float,float,float,bool,bool);
    void setNeighbors(bool[6]);
    virtual void updateVisuals();
    virtual void permanentPosition(int,int,int);
    virtual void draw();
    virtual void drawSilhouette();
    virtual void drawOutline();
    virtual void drawOutlineVolume();
    virtual void drawShadow();
    virtual void drawShadowVolume();
    virtual bool hasShadow();
    virtual bool hasFace(int);
    virtual bool hasVisibleFace();
    virtual void setShadow(bool);
    virtual void setRelationToCam(bool,bool,bool);
};

#endif
