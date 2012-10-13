/*
 * keyboard.h
 * by Brian Handy
 * 1/23/12
 * header for keyboard input for cubior
 */
#ifndef KEYBOARD
#define KEYBOARD
 
    void playerFullscreen(bool);
    void playerLevelShadows(bool);
    void playerPause(int, bool);
    int getLastPause();

    void inputDown(unsigned char, int, int);
    void inputUp(unsigned char, int, int);
    void handleInput(unsigned char, bool);
    void sendCommands();
    void joystickCommands(int);
    void mergeInput(int);
    void specialInputDown(int, int, int);
    void specialInputUp(int, int, int);
    void handleSpecialInput(int, bool);
    //void joystickDown(unsigned int, int, int, int);
    //void handleJoystickInput(unsigned int, bool);
    void nextLevelPressed(bool);
    void lastLevelPressed(bool);

    void setJump(int, bool);
    void setLock(int, bool);
#endif 
