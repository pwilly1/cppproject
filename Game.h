#ifndef GAME_H
#define GAME_H
#include "GameObject.h"
#include <SDL3/SDL.h>
#include <iostream>

class Game {
public:

	Game();
	~Game();

	bool init();
	void render();
	void handleEvents();
	void run();
	void cleanup();

private:
	GameObject* player;
	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window = nullptr; //window pointer object that points to window created.
	bool running = false;



};

#endif