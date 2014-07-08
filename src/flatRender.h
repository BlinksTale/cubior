/*
 * flatRender.h
 * by Brian Handy
 * 1/20/12
 * header for 2d visuals for cubior
 */

#ifndef FLATRENDER
#define FLATRENDER

#include <string>

void display();
void displayMenu();
void displayFor(int); // individual views
void timerRenderLoop(int);

void initVisuals();
void initScenery();
void compressScenery();
void initMenu();
void initFlat(int, char**);
void fillScreenWithShadow();
void calcPlayer(int);
void drawAllShadows(int);

void preDrawPlayer(int);
float playerRotationMean(int);
void postDrawPlayer(int);

void drawPlayer(int);
void drawPlayerSilhouette(int);
void drawPlayerShadow(int);
void drawPlayerOutline(int);
void drawAllCubes(int);
void drawCube(int,int);
bool cubeWithinPlayerRange(int,int);
void drawCubeShadow(int);
void drawCubeOutline(int);
void drawGoal();
void drawGoalShadow();
void drawGoalOutline();
void drawMenu(int,bool);
void drawItems(bool);
void drawItemShadows();

void writeWordRangeRotation(std::string,int,int,float,float,int,int);
void writeWordIndividualRotation(std::string,int,int,float,float,int);
void writeWord(std::string,int,int,float,float);

void updatePlayerGraphic(int);
void updateCubeGraphic(int);
void updateGoalGraphic();

void setPerspective(int, int);
void setPlayerGraphic(int,int,int,int);
void setCubeGraphic(int,int,int,int,bool);
void setGoalGraphic(int,int,int);

void updateFlat();
void renderFlat();

void printStringFlat(char*,int,int,int);
void printStringFlat(char*,int,int);
void printString(char*,int,int,int);
bool getTiming();
void toggleFullscreen();
void toggleLevelShadows();

// Time keeping
int getFPS();
int getLastFPS();
float getFPSRate();
int getTimePassed();

#endif 
