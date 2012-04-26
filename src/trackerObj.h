/*
 * trackerObj.h
 * by Brian Handy
 * 4/24/12
 * Header for trackers, which are Cubes that follow things
 */

#ifndef TRACKEROBJ
#define TRACKEROBJ

class TrackerObj : public CubeObj {
    CubeObj* target;
    const static int followDist = 100;
    bool chasing;
  public:
    TrackerObj();
    void tick();
    bool getChasing();
    void setTarget(CubeObj*);
    CubeObj* getTarget() { return target; }
};

#endif 
