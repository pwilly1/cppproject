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

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("window", 800, 600, SDL_WINDOW_RESIZABLE);
	if (!window) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;  // Exit if window creation fails
	}
	renderer = SDL_CreateRenderer(window, NULL);

	player = new GameObject(100, 100, 50, 50, renderer, "C:/Users/prwil/source/repos/pwilly1/cppproject/resources/Heroes/Man/Naked/Idle.png");

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

			if (key == SDLK_W) { player->setVelocity(0, -player->getSpeed()); std::cout << "W key press test"; }
			if (key == SDLK_S) {
				player->setVelocity(0, player->getSpeed()); // Move down
			}
			if (key == SDLK_A) {
				player->setVelocity(-player->getSpeed(), 0); // Move left
			}
			if (key == SDLK_D) {
				player->setVelocity(player->getSpeed(), 0); // Move right
			}

			if (key == SDLK_ESCAPE) {
				std::cout << "Escape key pressed. Exiting game...\n";
				running = false;
			}
		}
		if (event.type == SDL_EVENT_KEY_UP) {
			SDL_Keycode key = event.key.key;
			if (key == SDLK_W || key == SDLK_S) player->setVelocity(player->getdx(), 0);
			if (key == SDLK_A || key == SDLK_D) player->setVelocity(0, player->getdy());

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
	Uint64 lastTime = SDL_GetTicks();

	while (running) {
		Uint64 currentTime= SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;
		handleEvents();
		player->update(deltaTime);
		render();
		
	}
	cleanup();
}

