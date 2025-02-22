#include <SDL3/SDL.h>
#include <iostream>
#include "Game.h"

int main() {
	Game game;
	if(game.init()) {

		game.run();
	}
	return 0;
}
