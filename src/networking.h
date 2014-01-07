/*
 * Networking header for Cubior
 * by Brian Handy
 * 5/25/13
 * Networking class for 3d platformer
 */

#ifndef NETWORKING
#define NETWORKING
#define enet_lib

#include <iostream>
#ifdef enet_lib
#include <enet/enet.h>
#endif
#include <string>
#include <vector>

using namespace std; // necessary for string

const bool networkingAllowed = true;

    void networkingInit();
#ifdef enet_lib
    int connectTo(string);
    void disconnectFrom(string);
    void networkTick();
    void pollFor(ENetHost *, ENetAddress);
    int findComma(int, string);
    string* splitByCharacter(string, string*, int, char);
    bool networkPriority();

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
    void setMomentum(int, vector<float>);
    vector<float> getMomentum(int);
    void setDirection(int,float);
    float getDirection(int);
    bool isConnected();

#endif
#endif

void initializeIpAddress();
void incrementIpAddress(int);
bool setIpAddress(int, int);
int getIpAddress(int);
string getIpAddress();