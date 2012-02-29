/*
 * flatRender.h
 * by Brian Handy
 * 1/20/12
 * header for 2d visuals for cubior
 */
#ifndef FLATRENDER
#define FLATRENDER

    void display();
    void initFlat(int, char**);
    void drawPlayer(int);
    void drawCube(int);
    void updatePlayerGraphic(int);
    void updateCubeGraphic(int);
    void setPlayerGraphic(int,int,int,int);
    void setCubeGraphic(int,int,int,int,bool);
    void updateFlat();
    void renderFlat();

#endif 
