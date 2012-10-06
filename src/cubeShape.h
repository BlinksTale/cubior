/*
 * cubeShape.h
 * by Brian Handy
 * 2/2/12
 * header for 3d model of cube
 */
#ifndef CUBESHAPE
#define CUBESHAPE

class CubeShape {
  
  protected:
    bool alternatingSpot, midFloor; // for tile color and sides drawn
    bool defaultHasShadow; // true later for players and goals
    bool neighbors[6]; // points to neighbors array of cubeObj
    float altDark;
    float r1,g1,b1,r2,b2,g2,r3,g3,b3;
    const static bool useNeighbors = true;
  public:
    virtual void initVisuals(float,float,float,float,float,float,float,bool,bool);
    void setNeighbors(bool[6]);
    virtual void updateVisuals();
    virtual void draw();
    virtual void drawSilhouette();
    virtual void drawShadow();
    virtual bool hasShadow();
};

#endif
