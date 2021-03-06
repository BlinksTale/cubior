/*
 * Networking header for Cubior
 * by Brian Handy
 * 5/25/13
 * Networking class for 3d platformer
 */

#ifndef NETWORKING
#define NETWORKING

#include <iostream>
#include <enet/enet.h>
#include <string>
#include <vector>

using namespace std; // necessary for string

const bool networkingAllowed = true;

    int connectTo(string);
    void disconnectFrom(string);
    void networkTick();
    void pollFor(ENetHost *, ENetAddress);
    int findComma(int, string);
    string* stringToArray(string, string*, int);

    void resetSlots();
    int getNextSlot(string[]);

    void setPosX(int);
    void setPosY(int);
    void setPosZ(int);
    int getPosX();
    int getPosY();
    int getPosZ();
    void setOnline(int);
    bool getOnline(int);
    void setMomentum(vector<float>);
    vector<float> getMomentum();
    void setDirection(float);
    float getDirection();
    bool isConnected();

#endif
