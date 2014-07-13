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

#include "cubeObj.h" // for neighbor objects

class CubeShape {
  
  protected:
    bool alternatingSpot, midFloor; // for tile color and sides drawn
    bool shadowState; // default is true later for player/goal
    bool directionalCulling, aboveCam, behindCam, leftCam; // extra face culling stuff
    bool neighbors[6]; // points to neighbors array of cubeObj
    CubeObj* neighborObjects[6]; // points to the neighbors themselves
    int permanentX, permanentY, permanentZ, material, posX, posY, posZ, cubeNumber;
    float altDark;
    float r1,g1,b1,r2,b2,g2,r3,g3,b3;
    const static bool useNeighbors = true;
    GLfloat myVertices[24], myColors[32], topColors[16];//, myShadowVertices[24];
    //GLfloat myShadowVertices[24];
    CubeObj* selfObj = NULL;
    float alph; // alpha value
  public:
    virtual GLubyte getIndex(int);
    virtual GLfloat getVertex(int);
    virtual GLfloat getColor(int);
    virtual GLfloat getTopColor(int);
    virtual GLfloat getShadowVertex(int);
    virtual void initVisuals(float,float,float,float,float,float,float,bool,bool);
    virtual void initVisuals();
    void setNeighbors(bool[6]);
    void setNeighborObjects(CubeObj*[6]);
    virtual void updatePos();
    virtual void updateVisuals();
    virtual void permanentPosition(int,int,int);
    virtual void setPos(int,int,int);
    virtual int getX();
    virtual int getY();
    virtual int getZ();
    
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
    virtual void removeDuplicateNeighbors();
    void setMaterial(int);
    int getMaterial();
    bool sameNeighborsAs(bool[6]);
    bool atLeastAllNeighborsOf(bool[6]);
    bool canRemove(CubeObj*);
    void setSelf(CubeObj*);
    
    int getCubeNumber();
    void setCubeNumber(int);
};

#endif
