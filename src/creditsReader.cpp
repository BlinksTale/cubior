// File to read credits in and set gameplay as such
// modified from mapReader
// which was modified from http://www.cplusplus.com/doc/tutorial/files/

// reading a text file
#include "creditsReader.h"
#include "gameplay.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>// for atoi
#include <stdio.h> // for fopen
using namespace std;

#define NULL 0

string* credits = NULL;

// Takes in a text file and returns lines of text as array of char*s
string* CreditsReader::readCredits(const string& s) {
    
	// To read file
  string file = s;
  string row;
  // To keep track of pos in file
  int iterator = 0;
  
  #ifdef __APPLE_CC__
  #else 
  if (credits != NULL) {
    // Alright, so… this breaks the game for Macs
    // so even though it looks like a memory leak otherwise,
    // to get the game functioning right now, we can only delete
    // the old map on Windows
    delete credits;
  }
  #endif
  credits = new string[200];
  // If a big array doesn't work, look into http://www.cplusplus.com/reference/vector/vector/
	
  // Stream in the file
  ifstream newCredits;
  newCredits.open(file.c_str());
	
  // Now find the ToC details
  if (newCredits.is_open()) {
    while (newCredits.good()) {
      getline(newCredits, row);
      
      // Start reading in lines of text!
      credits[iterator] = row;
      iterator++;
    }

    if (newCredits.good()) {
      cout << "It failed!" << endl;
    }
    newCredits.close();
  }

  else cout << "Unable to open " << file << endl; 
  
  // Then return it!
  return credits;
}
