/*
 * creditsReader.h
 * by Brian Handy
 * 12/5/12
 * Header for the .txt to credits converter
 */

#ifndef CREDITSREADER
#define CREDITSREADER

#include <string>

using namespace std;

class CreditsReader {
  public:
    static string* readCredits(const std::string&);
};

#endif 
