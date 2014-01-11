/*
 * Networking header for Cubior
 * by Brian Handy
 * 5/25/13
 * Networking class for 3d platformer
 */

#ifndef NETWORKING
#define NETWORKING

//#define enet_lib
#include <iostream>
#include <string>
#include <vector>
#include <SFML/Network.hpp>

#ifdef enet_lib
#include <enet/enet.h>
#endif

using namespace std; // necessary for string

const bool networkingAllowed = true;

void networkingInit();
void saveIp(sf::IpAddress);
void networkListen();
void networkBroadcast();

#ifdef enet_lib
void disconnectFrom(string);
void pollFor(ENetHost *, ENetAddress);
void enetListen();
void enetBroadcast();
#endif

int startHosting();
int connectTo(string);
void networkTick();
int findComma(int, string);
string stringUpTo(string, char, int);
string* splitByCharacter(string, string*, int, char);
bool networkPriority();
void processData();
void prepareData();

void resetSlots();
int getNextSlot(string[]);

void setPosX(int,int);
void setPosY(int,int);
void setPosZ(int,int);
int getPosX(int);
int getPosY(int);
int getPosZ(int);
void setOnline(int, bool);
bool getOnline(int);
void setRemoteOnline(int, bool);
void setMomentum(int, vector<float>);
vector<float> getMomentum(int);
void setDirection(int,float);
float getDirection(int);
bool isConnected();

void initializeIpAddress();
void incrementIpAddress(int);
bool setIpAddress(int, int);
int getIpAddress(int);
string getIpAddress();
void setLAN(bool);
bool getLAN();

#endif
