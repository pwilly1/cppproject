#include <SDL3/SDL.h>
#include <iostream>
#include "Game.h"

int main() {
	std::cout << "Main function started!" << std::endl;
	Game game;
	if(game.init()) {


		game.run();
	}
	return 0;
}
