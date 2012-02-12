/*
 * cubiorObj.h
 * by Brian Handy
 * 2/12/12
 * Header for any cubior in the game (npcs and players)
 */

#ifndef CUBIOROBJ
#define CUBIOROBJ

class CubiorObj : public CubeObj {
    float happiness;
    bool invincible;
  public:
    CubiorObj();
    float getHappiness() { return happiness; }
    void setHappiness(float);
    bool getInvincibility() { return invincible; }
    void setInvincibility(bool);

};

#endif 
