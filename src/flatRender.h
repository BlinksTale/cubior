/*
 * flatRender.h
 * by Brian Handy
 * 1/20/12
 * header for 2d visuals for cubior
 */
#ifndef FLATRENDER
#define FLATRENDER

    void display();
    void displayFor(int); // individual views
    void initFlat(int, char**);
    void drawPlayer(int);
    void drawCube(int);
    void drawGoal();
    void updatePlayerGraphic(int);
    void updateCubeGraphic(int);
    void updateGoalGraphic();
    void setPerspective(int, int);
    void setPlayerGraphic(int,int,int,int);
    void setCubeGraphic(int,int,int,int,bool);
    void setGoalGraphic(int,int,int);
    void updateFlat();
    void renderFlat();
    void printString(char*,int,int,int);
    bool getTiming();

#endif 
