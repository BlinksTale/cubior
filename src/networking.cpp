/*
 * Networking for Cubior
 * by Brian Handy
 * 5/25/13
 * Networking class for 3d platformer
 */

// For Enet: (not sure how much still applies now that enet is gone)
// had to add directory "include" to VC++ Project Properties > Directories
// had to add enet.lib, winmm.lib, ws2_32.lib to Project Properties > Linker > Input top bar

// This now works with itself, but not with a copy on the same machine. How about another machine?

#include <cstdlib> // atoi
#include <SFML/Network.hpp> // for networking

#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <vector>
#include "networking.h"
#include "gameplay.h" // only for CubiorCount
#include <sstream> // for ostringstream


using namespace std;

// Preset variables
bool reconnectOnDisconnect = true;
const int onlinePlayerMax = 4;//16;
const int localPlayerMax = 4;

// Tools
/*time_t startTime = time(0);
int totalMilliseconds = 0;
const int timesPerSec = 1;
int millisecondModulo = (int)(1000.0/timesPerSec);
*/
// Game instance variables
bool isHost = false; // one host collects and redistributes all data
bool lan = false;

// Declare SFML Networking Variables
sf::UdpSocket socketItself;
unsigned short socketPort = 54000;
sf::Packet packet;
sf::IpAddress incomingIp, myIp, bannedIp;
unsigned short incomingPort;

sf::IpAddress senderAddress;

const int messageSize = 1024;
char message[messageSize];
char quarterMessage[localPlayerMax][messageSize/4];

// Declare my code's variables
string role;
bool choseHost;
bool keepLooping;
string oldAddress = "127.0.0.1";
bool connected = false;
int ticks = 0; // the game with more ticks gets higher priority
int remoteTicks = 0; // the other game's ticks
bool hostExists = false;
string newData, latestData, nextMessage, lastMessage, secondToLastMessage, knownIps[16];
sf::IpAddress knownIpObjects[16];
int knownIpSize = 0;
// Recieving
bool isOnline[onlinePlayerMax];
int posX[onlinePlayerMax], posY[onlinePlayerMax], posZ[onlinePlayerMax];
vector< vector <float> > joy (localPlayerMax, vector<float> (2, 0));
bool jump[onlinePlayerMax];
// Sending
bool myOnline[localPlayerMax];
int myPosX[localPlayerMax], myPosY[localPlayerMax], myPosZ[localPlayerMax];
vector< vector <float> > myJoy (localPlayerMax, vector<float> (2, 0));
bool myJump[localPlayerMax];
bool onlineStatus[cubiorCount];
//vector<float> momentum (3, 0); // use onlinePlayerMax
vector< vector <float> > momentum (onlinePlayerMax, vector<float> (3, 0)); // multidimensional vector
//vector<float> myMomentum (3, 0);
vector< vector <float> > myMomentum (localPlayerMax, vector<float> (3, 0));
float direction[onlinePlayerMax];
float myDirection[localPlayerMax];
int landedOn[localPlayerMax];
int myLandedOn[localPlayerMax];
// Save IP sources
int playerIp[onlinePlayerMax];
int playerIpNew[onlinePlayerMax];
int playerIpDisconnect[onlinePlayerMax];

int currentMessageSlot = 0; // for reading in data from a message

void networkingInit() {
    for (int i=0; i<onlinePlayerMax; i++) {
        isOnline[i] = false;
    }
    initializeIpAddress();
    
    if (socketItself.bind(socketPort) != sf::Socket::Done) {
        // error!
    }
    socketItself.setBlocking(false);
    
    for (int i=0; i<16; i++) { // 16 is knownIps length
        knownIps[i] = "";
    }
    knownIpSize = 0;
}


// Try to save all IPs that come in on the right port
void saveIp(sf::IpAddress incomingIpObject) {
    string incomingIpString = incomingIpObject.toString();
    // Check if already stored
    for (int i=0; i<16; i++) { // 16 is knownIps length
        if (strcmp(knownIps[i].c_str(),"") == 0) {
            knownIps[i] = incomingIpString;
            knownIpObjects[i] = incomingIpObject;
            knownIpSize = knownIpSize >= i+1 ? knownIpSize : i+1;
            return;
        } else if (strcmp(knownIps[i].c_str(),incomingIpString.c_str()) == 0) {
            return;
        }
    }
    // Otherwise... must be full and it's not in there?
}

void dropPlayer(int i) {
    isOnline[i]  = false;
    playerIp[i] = -1;
    playerIpDisconnect[i] = 0;
}

void dropIdlePlayers() {
    
    for (int i=0; i<onlinePlayerMax; i++) {
        // For all players known as online still
        if (playerIp[i] != -1) {
            // If we just got the IP, reset disconnect count
            if (incomingIp.toInteger() == playerIpNew[i]) {
                playerIpDisconnect[i] = 0;
            } else {
                // no new message for this player
                playerIpDisconnect[i]++;

                // Timeout or this player not being sent from original source anymore
                if (playerIpDisconnect[i] > 500 || playerIpNew[i] == -1) {
                    //cout << "Dropping player " << i << endl;
                    dropPlayer(i);
                }
            }
            //cout << "PlayerIp " << i << " is " << playerIp[i] << " with disconnect " << playerIpDisconnect[i] << " and ticks " << ticks << endl;

        }
    }
    
    incomingIp = sf::IpAddress::None;

}

void networkListen() {
    
    socketItself.receive(packet, incomingIp, incomingPort);
    
    if (incomingPort == socketPort &&
        incomingIp.toInteger() != myIp.toInteger() &&
        incomingIp.toInteger() != bannedIp.toInteger()) {

        newData = "";
        packet >> newData;

        if (newData.length() > 0 &&
            strcmp(newData.c_str(), latestData.c_str()) != 0) {
            latestData = newData;
            readData();
        }

        if (isHost && incomingIp.toInteger() != bannedIp.toInteger()) {
            saveIp(incomingIp.toString());
        }
        
    }
    
    dropIdlePlayers();
    
    packet.clear();
}

void networkBroadcast() {
    
    writeData();
    
    // Must make a version of nextMessage only up to the fourth semicolon
    string abreviatedNextMessage = stringUpTo(nextMessage, ';', 4);
    //cout << "Abreviated is " << abreviatedNextMessage << endl;
    
    if (ticks % 10 == 0 || // so it gets sent every ten ticks instead of when getCubiorsOnline() == 0
        (strcmp(abreviatedNextMessage.c_str(), lastMessage.c_str()) != 0 && // or when different from last 2 messages
        strcmp(abreviatedNextMessage.c_str(), secondToLastMessage.c_str()) != 0)) {

        packet << nextMessage;
        
        // Send data
        if (isHost) {
            string allIPs = "";
            for (int i=0; i<knownIpSize; i++) {
                socketItself.send(packet, knownIpObjects[i], socketPort);
                allIPs += knownIpObjects[i].toString() + ", ";
            }
          cout << "Known IP size is " << knownIpSize << " of " << allIPs<< endl;
        } else if (lan) {
            socketItself.send(packet, sf::IpAddress::Broadcast, socketPort);
        } else {
            socketItself.send(packet, senderAddress, socketPort); // sf::IpAddress::Broadcast
        }
    
        secondToLastMessage = lastMessage;
        lastMessage = abreviatedNextMessage;
        packet.clear();

    }

}

int startHosting() {
    cout << "Starting hosting" << endl;
    isHost = true;
    hostExists = true;
    connected  = true;
    
    return 0;
}

int connectTo(string newAddress)
{
    isHost = false;
    cout << "Trying to connect to " << newAddress << endl;
    if (!hostExists && !connected) {
        cout << "No host yet" << endl;
        
        // Shortcut for same is an empty string
        if (newAddress.compare("") == 0) {
            newAddress = oldAddress;
        }
      
        hostExists = true;
        oldAddress = newAddress;
        connected  = true;
    } else if (!connected) {
       connected = true;
    }
    
    return 0;
}

// Deconstruct our latestData
void readData() {
    /*
     * Split by player (separated by semicolon), then split by data (separated by comma)
     */
    
    //string str(latestData);
    string playerArray[onlinePlayerMax+2]; // +1 for ticks and +1 for level
    splitByCharacter(latestData, playerArray, onlinePlayerMax + 2, ';'); // +1 for ticks and +1 for level
    
    //cout << "Reading message data: " << latestData << endl;

    // Ticks and Level
    int potentialRemoteTicks = atoi(playerArray[onlinePlayerMax].c_str());
    //if (abs(remoteTicks - potentialRemoteTicks) > 5) {
      remoteTicks = potentialRemoteTicks;
    //} else {
    //  bannedIp = incomingIp;
    //}
    int theirLevel = atoi(playerArray[onlinePlayerMax+1].c_str());
    if (!getGameplayFirstRunning() && getGameplayRunning() && ticks > 10 && theirLevel > getLevelNum()) {
        loadLevel(theirLevel);
        cout << "Their level is " << theirLevel << " and ours is " << getLevelNum() << endl;
    }
    
    for (int h=0; h<onlinePlayerMax; h++) {
        
        // Reset all current incoming IPs
        // we'll check against this next time to see who dropped
        if (playerIpNew[h] != -1) {
            playerIpNew[h] = -1;
        }
        
        if (playerArray[h].compare("\0") != 0 && playerArray[h].compare("") != 0) {
            /*
             * Split by data here
             */
            
            // Save this player to active players coming from this IP
            playerIp[h] = incomingIp.toInteger();
            playerIpNew[h] = incomingIp.toInteger();
            
            float momentumX, momentumY, momentumZ;
            string str(playerArray[h]);
            const int resultSize = 10;
            string dataArray[resultSize];
            splitByCharacter(str, dataArray, resultSize, ',');
            
            //for (int i=0; i<resultSize; i++) {
            //  cout << "Line " << i << " is " << dataArray[i] << endl;
            //}
            
            for (int v = 0; v<resultSize; v++) {
                //cout << endl << "Getting msg[" << v << "] = " << dataArray[v];
            }
            if (resultSize > 0) {
                //cout << endl;
            }
            
            resetSlots();
            // Find which player is sending data, and update it
            int p        = getNextSlot(dataArray);
            isOnline[p]  = true;
            /*posX[p]      = getNextSlot(dataArray);
            posY[p]      = getNextSlot(dataArray);
            posZ[p]      = getNextSlot(dataArray);
            momentumX    = getNextSlot(dataArray);
            momentumY    = getNextSlot(dataArray);
            momentumZ    = getNextSlot(dataArray);
            direction[p] = getNextSlot(dataArray);
            landedOn[p]  = getNextSlot(dataArray);
             */
            float joyX      = getNextSlot(dataArray);
            float joyY      = getNextSlot(dataArray);
            //jump[p]      = getNextSlot(dataArray);
            momentumY = getNextSlot(dataArray);
            momentumX = 0;
            momentumZ = 0;
            
            float joyArray[] = { joyX, joyY };
             std::vector<float> newJoy (joyArray, joyArray + sizeof(joyArray) / sizeof(float) );
             joy[p].swap(newJoy);
            
            float momentumArray[] = { momentumX, momentumY, momentumZ };
            std::vector<float> newMomentum (momentumArray, momentumArray + sizeof(momentumArray) / sizeof(float) );
            momentum[p].swap(newMomentum);
            
            //cout << " Made the positions " << posX << " / " << posY << " / " << posZ << endl;
        }
    }
}

// Opposite of process data, get it ready to send
void writeData() {
    //cout << "Send > ";
    //cin.getline(message, messageSize);
    //message = memcpy(message,newString.c_str(),newString.size());ticks;
    //itoa(ticks, message, 10);
    //string ticksString = to_string(ticks);
    //message = memcpy(message, ticksString.c_str(), ticksString.size());
    
    // Clear message
    sprintf(message, "");
    
    //int miniMessageSize = 256;//1024 / 4;
    //int posY = sin(ticks/1000.0*3.14*2)*250+250; // fly up and down in 0 to 500 range
    // Loop through new messages for each online player
    // but send the packet once for every player
    for (int i=0; i<localPlayerMax; i++) {
        sprintf(quarterMessage[i], "");
        if (myOnline[i]) {
            
            sprintf(quarterMessage[i], "%d,%f,%f,%f",
                    i, myJoy[i].at(0), myJoy[i].at(1), momentum[i].at(1));//myJump[i]);
            /*
            // Add one player's data
            sprintf(quarterMessage[i], "%d,%d,%d,%d,%f,%f,%f,%f,%d",
                    i, // send the player's number first, essentially the id
                    myPosX[i], myPosY[i], myPosZ[i],
                    myMomentum[i].at(0), myMomentum[i].at(1), myMomentum[i].at(2),
                    myDirection[i], myLandedOn[i]);*/
        }
    }
    // Send all player data
    sprintf(message, "%s;%s;%s;%s;%d;%d",
            quarterMessage[0], quarterMessage[1], quarterMessage[2], quarterMessage[3], ticks % 1000, getLevelNum());
    nextMessage = message; // std::string automatically converts from char* to string
    
    //cout << "Writing message data: " << message << endl;
}

bool networkPriority() { // are we the oldest network?
    cout << "Checking if our " << ticks << " is greater than their " << remoteTicks << endl;
    return ticks % 1000 > remoteTicks;
}

void resetSlots() {
  currentMessageSlot = 0;
}

int getNextSlot(string dataArray[]) {
  int result = atoi(dataArray[currentMessageSlot].c_str());
  currentMessageSlot++;
  return result;
}

string stringUpTo(string str, char character, int characterNumber) {
    int leftComma = 0;
    int rightComma = 0;
    string results;
    
    // Get everything after the current comma, and find the next comma
    // then take the slice between the two as the data for slot i
    for (int i=0; i<characterNumber; i++) {
        string focus = str.substr(leftComma, str.length()-leftComma);
        rightComma = (int)focus.find(character);
        results += str.substr(leftComma, rightComma);
        leftComma += rightComma+1;
    }
    return results;
}

// Splits a string into an array based on its commas
string* splitByCharacter(string str, string* result, int resultSize, char character) {
    int leftComma = 0;
    int rightComma = 0;

    // Populate empty results array
    for (int i=0; i<resultSize; i++) {
        result[i] = "\0";
    }

    // Get everything after the current comma, and find the next comma
    // then take the slice between the two as the data for slot i
    for (int i=0; i<resultSize; i++) {
        string focus = str.substr(leftComma, str.length()-leftComma);
        rightComma = focus.find(character);
        result[i] = str.substr(leftComma, rightComma);
        if (rightComma == string::npos) {
          return result;
        }
        leftComma += rightComma+1;
    }
    return result;
}

int findComma(int commaNum, string text) {
  for (int i=0; i<text.length(); i++) {
    if (text.at(i) == ',') {
      commaNum--;
      if (commaNum == 0) {
        return i;
      }
    }
  }
  return -1;
}

void setOnline(int i, bool b) {
    myOnline[i] = b;
}
void setPosX(int i, int pos) {
    myPosX[i] = pos;
}
void setPosY(int i, int pos) {
    myPosY[i] = pos;
}
void setPosZ(int i, int pos) {
    myPosZ[i] = pos;
}
int getPosX(int i) {
    return posX[i];
}
int getPosY(int i) {
    return posY[i];
}
int getPosZ(int i) {
    return posZ[i];
}
bool getOnline(int i) {
    return isOnline[i];
}
void setRemoteOnline(int i, bool b) {
    isOnline[i] = b;
    if (!b) {
        dropPlayer(i);
    }
}

void setMomentum(int i, vector<float> m) {
  myMomentum[i] = m;
}
vector<float> getMomentum(int i) {
  return momentum[i];
}

void setDirection(int i, float f) {
  myDirection[i] = f;
}
float getDirection(int i) {
  return direction[i];
}

void setLandedOn(int i, int f) {
  myLandedOn[i] = f;
}
int getLandedOn(int i) {
  return landedOn[i];
}
void setJoy(int i, float jX, float jY) {
    myJoy[i].at(0) = jX;
    myJoy[i].at(1) = jY;
}
vector<float> getJoy(int i) {
    return joy[i];
}
void setJumpOnline(int i, bool b) {
    myJump[i] = b;
}
bool getJumpOnline(int i) {
    return jump[i];
}

// The main loop, called repeatedly
void networkTick() {
    
    //cout << "Network is " << (connected ? "" : "not ") << "connected" << endl;
  
    ticks++;

    // Listen for any info for server and client
    networkListen();

    /*time_t currentTime = time(0);
    int millisecondsPassed = difftime(currentTime, startTime);
    startTime = currentTime;
    
    cout << "Milliseconds passed is " << millisecondsPassed << " to be added to " << totalMilliseconds << endl;
    totalMilliseconds += millisecondsPassed;
    
    // No packages will be sent until your game is started
    if (getStarted() && totalMilliseconds > millisecondModulo) { //  && !getCubiorOnline(myPlayer)
        totalMilliseconds = totalMilliseconds % millisecondModulo; // 33 means it gets called about 30 times/sec*/
    if (getStarted() && ticks % 1 == 0) {
        // Then send data if client
        //if (!choseHost) {
        writeData();
      networkBroadcast();
    }
    //}
    
}

bool isConnected() {
    return connected;
}


// Universal

int addressSlot[4];

void initializeIpAddress() {
    addressSlot[0] = 192;
    addressSlot[1] = 168;
    addressSlot[2] = 0;
    addressSlot[3] = 9;
    senderAddress = sf::IpAddress(getIpAddress());
}

bool setIpAddress(int slot, int value) {
    if (value >= 0 && value <= 255) {
        addressSlot[slot] = value;
        senderAddress = sf::IpAddress(getIpAddress());
        return true;
    }
    return false;
}

void incrementIpAddress(int slot) {
    if (addressSlot[slot] < 255) {
        addressSlot[slot]++;
    } else {
        addressSlot[slot] = 0;
    }
    senderAddress = sf::IpAddress(getIpAddress());
}

int getIpAddress(int slot) {
    return addressSlot[slot];
}
string getIpAddress() {
    //char result[50];
    //sprintf(result, "%d.%d.%d.%d", addressSlot[0], addressSlot[1], addressSlot[2], addressSlot[3]);
    string result = "";
    ostringstream ss;
    for (int i=0; i<4; i++) {
        int val = addressSlot[i];
        string part2 = i < 3 ? "." : "";
        ss << val << part2;
    }
    result += ss.str();
    return result;
}

void setLAN(bool b) {
    lan = b;
    
    if (lan) {
        myIp = sf::IpAddress::getLocalAddress();
    } else {
        myIp = sf::IpAddress::getPublicAddress();
    }
    cout << "LAN is " << lan << " and myIp is " << myIp.toString() << endl;
}
bool getLAN() {
    return lan;
}