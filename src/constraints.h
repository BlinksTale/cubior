/*
 * constraints.h
 * by Brian Handy
 * 1/15/13
 * Variables that gameplay and maps depend on, limitations of engine/levels
 */

#ifndef CONSTRAINTS
#define CONSTRAINTS

const int mapEdge = 4;
const int wallCheckRadius = 3;

const int playableWidth = 100;
const int playableHeight= 100;
const int playableDepth = 100;
const int maxWidth = playableWidth + mapEdge*2;
const int maxHeight= playableHeight + mapEdge*2;
const int maxDepth = playableDepth + mapEdge*2;
const int tileSize = 100;
const int totalLevels = 12; // for now, update this when new levels are added
const int padding = 2;

#endif 