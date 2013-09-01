/*
 * Networking for Cubior
 * by Brian Handy
 * 5/25/13
 * Networking class for 3d platformer
 * most code copied from enet tutorials
 */

// NetworkTest.cpp : main project file.
// Most code copied from Enet tutorial
// Copying done by Brian Handy, 5/4/13

// had to add directory "include" to VC++ Project Properties > Directories
// had to add enet.lib, winmm.lib, ws2_32.lib to Project Properties > Linker > Input top bar

// This now works with itself, but not with a copy on the same machine. How about another machine?

#include <cstdlib> // atoi
#include <iostream>
#include <enet/enet.h>
#include <string>
#include <cmath>
#include <vector>
#include "networking.h"
#include "gameplay.h" // only for CubiorCount

using namespace std;

// Preset variables
bool reconnectOnDisconnect = true;
const int onlinePlayerMax = 4;//16;
const int localPlayerMax = 4;

// Game instance variables
bool isHost = false; // one host collects and redistributes all data

// Declare Enet Variables
ENetAddress addressServer;
ENetAddress addressClient;
ENetHost * server;
ENetHost * client;
ENetPeer * peer;
const int messageSize = 1024;
char message[messageSize];

// Declare my code's variables
string role;
bool choseHost;
bool keepLooping;
string oldAddress = "127.0.0.1";
bool connected = false;
int ticks = 0;
bool hostExists = false;
string latestData;
// Recieving
bool isOnline[onlinePlayerMax];
int posX[onlinePlayerMax], posY[onlinePlayerMax], posZ[onlinePlayerMax];
// Sending
bool myOnline[localPlayerMax];
int myPosX[localPlayerMax], myPosY[localPlayerMax], myPosZ[localPlayerMax];
bool onlineStatus[cubiorCount];
//vector<float> momentum (3, 0); // use onlinePlayerMax
vector< vector <float> > momentum (onlinePlayerMax, vector<float> (3, 0)); // multidimensional vector
//vector<float> myMomentum (3, 0);
vector< vector <float> > myMomentum (localPlayerMax, vector<float> (3, 0));
float direction[onlinePlayerMax];
float myDirection[localPlayerMax];

int currentMessageSlot = 0; // for reading in data from a message

void networkingInit() {
  for (int i=0; i<onlinePlayerMax; i++) {
    isOnline[i] = false;
  }
}

void pollFor(ENetHost * host, ENetAddress address) {
  // Host Polling from Enet
    ENetEvent event;
    bool updatePos = false;
    // Wait up to 1000 milliseconds for an event.
    while (enet_host_service (host, & event, 1) > 0) // used to be 1000, not 1 (update 1/sec. Now it's 1000/sec)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            //cout << "A new client connected from " << event.peer -> addressServer.host <<
            //  ":" << event.peer -> addressServer.port << ".\n";
            cout << event.peer -> address.host << " joined the game\n";
            // Store any relevant client information here.
            //event.peer -> data = "Client information";
            connected = true;
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            //cout << "A packet of length " << event.packet -> dataLength <<
            //  " containing " << event.packet -> data << " was received from " <<
            //  event.peer -> data << " on channel " << event.channelID << ".\n";

            //cout << event.peer -> data << ": " << event.packet -> data << "\n";
            // Save some data for later
            latestData = ((const char*)event.packet -> data);
            updatePos = true;
            // Clean up the packet now that we're done using it.
            enet_packet_destroy (event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            cout << event.peer -> data << " disconected.\n";
            // Reset the peer's client information.
            event.peer -> data = NULL;
            connected = false;
            break;
        default:
            cout << "No new data" << endl;
        }
    }
    
    if (updatePos) {
      /*
       * Split by player (separated by semicolon), then split by data (separated by comma)
       */

      string str(latestData);
      string playerArray[onlinePlayerMax];
      splitByCharacter(str, playerArray, onlinePlayerMax, ',');

      for (int h=0; h<onlinePlayerMax; h++) {
        if (playerArray[h].compare("\0") != 0) {
          /*
           * Split by data here
           */

          float momentumX, momentumY, momentumZ;
          string str(playerArray[h]);
          const int resultSize = 10;
          string dataArray[resultSize];
          splitByCharacter(str, dataArray, resultSize, ',');
      
          //for (int i=0; i<resultSize; i++) {
          //  cout << "Line " << i << " is " << dataArray[i] << endl;
          //}
      
          for (int v = 0; v<resultSize; v++) {
            cout << endl << "Getting msg[" << v << "] = " << dataArray[v];
          }
          if (resultSize > 0) {
            cout << endl;
          }

          resetSlots();
          // Find which player is sending data, and update it
          int p        = getNextSlot(dataArray);
          isOnline[p]  = true;
          posX[p]      = getNextSlot(dataArray);
          posY[p]      = getNextSlot(dataArray);
          posZ[p]      = getNextSlot(dataArray);
          momentumX    = getNextSlot(dataArray);
          momentumY    = getNextSlot(dataArray);
          momentumZ    = getNextSlot(dataArray);
          direction[p] = getNextSlot(dataArray);
      
          float momentumArray[] = { momentumX, momentumY, momentumZ };
          std::vector<float> newMomentum (momentumArray, momentumArray + sizeof(momentumArray) / sizeof(float) );
          momentum[p].swap(newMomentum);

          //cout << " Made the positions " << posX << " / " << posY << " / " << posZ << endl;
        }
      }
    }

    // Get ready for next loop!
    enet_packet_destroy(event.packet);
}

void resetSlots() {
  currentMessageSlot = 0;
}

int getNextSlot(string dataArray[]) {
  int result = atoi(dataArray[currentMessageSlot].c_str());
  currentMessageSlot++;
  return result;
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

// The main loop, called repeatedly
void networkTick() {
    
    //cout << "Network is " << (connected ? "" : "not ") << "connected" << endl;
  
    ticks++;

    // Listen for any info for server and client
    //if (choseHost) {
        pollFor(server, addressServer);
    //} else {
        pollFor(client, addressClient);
    //}
    
    // If you need to, feel free to reconnect on a connection loss
    if (!connected && reconnectOnDisconnect) {
        connectTo(oldAddress);
    }
    
    // Then send data if client
    //if (!choseHost) {
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
        if (myOnline[i]) {
          sprintf(message, "%d,%d,%d,%d,%f,%f,%f,%f;", 
            i, // send the player's number first, essentially the id
            myPosX[i], myPosY[i], myPosZ[i],
            myMomentum[i].at(0), myMomentum[i].at(1), myMomentum[i].at(2),
            myDirection[i]);
        }

        ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);

        //cout << "Sending msg " << message << endl;

        // No packages will be sent until your game is started
        if (strlen(message) > 0 && getStarted()) { //  && !getCubiorOnline(myPlayer)
            enet_peer_send(peer, 0, packet);
        }
      }
    //}
    
}

int connectTo(string newAddress)
{
    //cout << "Trying to connect to " << newAddress << endl;
    if (!hostExists && !connected) {
        //cout << "No host yet" << endl;
        // Initialize Enety
        if (enet_initialize () != 0)
        {
            fprintf (stderr, "An error occurred while initializing ENet.\n");
            return EXIT_FAILURE;
        }
        atexit (enet_deinitialize);
        //cout << "Setup the exit route" << endl;

    //if (choseHost) {

        // Bind the server to the default localhost.
        // A specific host address can be specified by
        // enet_address_set_host (& addressServer, "x.x.x.x");
        addressServer.host = ENET_HOST_ANY;
        // Bind the server to port 1234.
        addressServer.port = 1234;
        server = enet_host_create (& addressServer, // the address to bind the server host to
                                     32,      // allow up to 32 clients and/or outgoing connections
                                      2,      // allow up to 2 channels to be used, 0 and 1
                                      0,      // assume any amount of incoming bandwidth
                                      0       // assume any amount of outgoing bandwidth
                                  );
        if (server == NULL)
        {
            fprintf (stderr, 
                     "An error occurred while trying to create an ENet server host.\n");
            exit (EXIT_FAILURE);
        }

    //} else {
        client = enet_host_create (NULL, // create a client host
                    1, // only allow 1 outgoing connection
                    2, // allow up 2 channels to be used, 0 and 1
                    57600 / 8, // 56K modem with 56 Kbps downstream bandwidth
                    14400 / 8  // 56K modem with 14 Kbps upstream bandwidth
                    );
        if (client == NULL)
        {
            fprintf (stderr, 
                     "An error occurred while trying to create an ENet client host.\n");
            exit (EXIT_FAILURE);
        }

        // Shortcut for same is an empty string
        if (newAddress.compare("") == 0) {
          newAddress = oldAddress;
        }

        enet_address_set_host (& addressClient, newAddress.c_str());
        addressClient.port = 1234;
        

        peer = enet_host_connect(client, & addressClient, 2, 0);

        if (peer == NULL) {
            fprintf (stderr,
                     "No available peers for initializing an ENet connection");
            exit (EXIT_FAILURE);
        }
        //}

        // Main Loop
        //keepLooping = true;
        //while (keepLooping) {
        //    tick();
        //}
    
        hostExists = true;
        oldAddress = newAddress;
        connected  = true;
    } else if (!connected) {
        peer = enet_host_connect(client, & addressClient, 2, 0);
        
        if (peer == NULL) {
            fprintf (stderr,
                     "No available peers for initializing an ENet connection");
            exit (EXIT_FAILURE);
        }
        connected = true;
    }

    return 0;
}

void disconnectFrom(string newAddress) {
    // Finish using Enet
    enet_host_destroy(client);
    enet_host_destroy(server);

}

bool isConnected() {
    return connected;
}