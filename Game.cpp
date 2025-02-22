#include <SDL3/SDL.h>
#include <iostream>
#include "Game.h"
#include "GameObject.h"


Game::Game() {
	renderer = nullptr;
	window = nullptr;
	running = false;
}

bool Game::init() {
	running = true;
	player = new GameObject(100, 100, 50, 50, renderer, "resources/Heroes/Man/Naked/Idle.png");
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("window", 800, 600, SDL_WINDOW_RESIZABLE);
	if (!window) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;  // Exit if window creation fails
	}
	renderer = SDL_CreateRenderer(window, NULL);
	return true;
}

Game::~Game() {
	cleanup();  // Ensure proper cleanup when the game object is destroyed
}



void Game::render() {

	SDL_SetRenderDrawColor(renderer, 60, 34, 15, 255);
	SDL_RenderClear(renderer);

	player->render(renderer);

	SDL_RenderPresent(renderer);
}

void Game::handleEvents() {
	if (SDL_PollEvent(&event) > 0) {
		if (event.type == SDL_EVENT_QUIT) {
			running = false;
		}

		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			SDL_MouseButtonFlags button = event.button.button;

			if (button == SDL_BUTTON_LEFT) {
				std::cout << "left mouse pressed\n";
			}
			if (button == SDL_BUTTON_MIDDLE) {
				std::cout << "middle mousepressed\n";
			}
			if (button == SDL_BUTTON_RIGHT) {
				std::cout << "right mouse pressed\n";
			}
		}

		if (event.type == SDL_EVENT_KEY_DOWN) {

			SDL_Keycode key = event.key.key;

			if (key == SDLK_W) {
				std::cout << "w key pressed\n";
			}
			if (key == SDLK_S) {
				std::cout << "S key pressed\n";
			}
			if (key == SDLK_D) {
				std::cout << "d key pressed\n";
			}
			if (key == SDLK_A) {
				std::cout << "a key pressed\n";
			}
			if (key == SDLK_ESCAPE) {
				std::cout << "Escape key pressed. Exiting game...\n";
				running = false;
			}
		}
		if (event.type == SDL_EVENT_KEY_UP) {
			SDL_Keycode key = event.key.key;

			std::cout << "Key Released: " << SDL_GetKeyName(key) << std::endl;
		}

	}

}

void Game::cleanup() {
	delete player;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();


}

void Game::run() {
	while (running) {
		handleEvents();
		render();
		
	}
	cleanup();
}

