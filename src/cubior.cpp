/*
 * Cubior
 * by Brian Handy
 * 1/13/12
 * Main class for 3d platformer
 */
#include <iostream>
#include "visuals.h"
#include "gameplay.h"
using namespace std;

#include <SFML/Graphics.hpp>

#include <iostream>

//int main()
//{
//}

// Gets called as often as possible
void tick() {
  //gameplayLoop();
  //updateGraphics(x);
  //render();
}

int main(int argc, char** argv) {
  gameplayStart("./maps/cubiorMap0.cubior");
  initRender(argc, argv); // nothing gets called after this
  //tick();
  
    /*sf::Clock Clock;
    while (Clock.GetElapsedTime() < 15.f)
    {
		//sf::Event::Joystick::update(); // must be called once per cycle since no SFML window
        std::cout << Clock.GetElapsedTime() << std::endl;
		//std::cout << sf::Joystick::isConnected(0) << std::endl;
		float phil = (sf::Event::JoyMoveEvent()).Position;
        std::cout <<  phil << std::endl;
        sf::Sleep(0.5f);
    }*/

    //sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    //sf::CircleShape shape(100.f);
    //shape.setFillColor(sf::Color::Green);

	//sf::Clock Clock;
	
	/*
	AND THE JOYSTICKS AREEEEE:
	For me/now at least,
	Controller 1: Joystick 1 // wired
	Controller 2: Joystick 2 // wireless 1
	Controller 3: Joystick 0 // wireless 2
	Controller 4: Joystick 3 // wireless 3

	Axis:
    X: Left Stick X (-100 left to 100 right)
	Y: Left Stick Y (-100 up to 100 down)
	Z: L and R (99 L to -99 R)
	R: Right Y (-100 up)
	U: Right X (-100 left)
	V: -100 locked?
	PovX: Dpad Y up (100 up!!!! to 0 for down?) and D-pad X for sure (-92 left to 92 right)
	PovY: Dpad Y down (100 down?) and Dpad X too? (38 left to 70 right?)
    */
	/*cout << "PROGRAM INITIATED" << endl;
	while (true) {
		sf::Joystick::update();
		if (sf::Joystick::isButtonPressed(1,0)) {
			for (int i=0; i<8; i++) {
				cout << "Joystick  " << i << ": " << sf::Joystick::isConnected(i) << endl;
				cout << "stats for " << i << ": " << sf::Joystick::getAxisPosition(i, sf::Joystick::X) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::Y) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::Z) << ", " << sf::Joystick::getAxisPosition(i, sf::Joystick::R) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::U) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::V) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::PovX) << ", "  << sf::Joystick::getAxisPosition(i, sf::Joystick::PovY) << endl;
			}
			cout << "----" << endl;
		}
    }*/

    return 0;
}



