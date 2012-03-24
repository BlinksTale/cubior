/*
 * keyboard.h
 * by Brian Handy
 * 1/23/12
 * header for keyboard input for cubior
 */
#ifndef KEYBOARD
#define KEYBOARD

    void inputDown(unsigned char, int, int);
    void inputUp(unsigned char, int, int);
    void handleInput(unsigned char, bool);
    void sendCommands();
    void specialInputDown(int, int, int);
    void specialInputUp(int, int, int);
    void handleSpecialInput(int, bool);
    void joystickDown(unsigned int, int, int, int);
    void handleJoystickInput(unsigned int, bool);

#endif 
