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
    void initVisuals();
    void initFlat(int, char**);
    void fillScreenWithShadow();
    void calcPlayer(int);
    void drawAllShadows();
    void preDrawPlayer(int);
    float playerRotationMean(int);
    void postDrawPlayer(int);
    void drawPlayer(int);
    void drawPlayerSilhouette(int);
    void drawPlayerShadow(int);
    void drawPlayerOutline(int);
    void drawCube(int,int);
    void drawCubeShadow(int);
    void drawCubeOutline(int);
    void drawGoal();
    void drawGoalShadow();
    void drawGoalOutline();
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
    void toggleFullscreen();
    void toggleLevelShadows();

#endif 
