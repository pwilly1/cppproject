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

	SDL_DisplayID displayID = SDL_GetPrimaryDisplay();

	//  Get pointer to display mode (SDL3 version)
	const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(displayID);

	//  Check if it failed
	if (!displayMode) {
		std::cout << "Failed to get display mode: " << SDL_GetError() << std::endl;
		return false;
	}

	screenWidth = displayMode->w;
	screenHeight = displayMode->h;

	
	window = SDL_CreateWindow("My Game", 600, 800, SDL_WINDOW_RESIZABLE);
	if (!window) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;  // Exit if window creation fails
	}
	renderer = SDL_CreateRenderer(window, NULL);

	SDL_SetRenderLogicalPresentation(renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);


	backgroundTexture = IMG_LoadTexture(renderer, "../../../resources/Cave Tiles/Cave Tiles.png");
	if (!backgroundTexture) {
		std::cout << "Failed to load background: " << SDL_GetError() << std::endl;
	}

	player = new GameObject(100, 100, renderer, "../../../resources/Heroes/Man/Naked/Idle.png");

	return true;
}

Game::~Game() {
	cleanup();  // Ensure proper cleanup when the game object is destroyed
}



void Game::render() {

	SDL_SetRenderDrawColor(renderer, 60, 34, 15, 255);
	SDL_RenderClear(renderer);

	SDL_FRect srcTile = { 16, 16, 16, 16 };  // (X, Y, Width, Height) in the tileset

	// Fill the entire screen with the tile
	for (int i = 0; i < screenWidth; i += 16) {  // Loop through screen width
		for (int j = 0; j < screenHeight; j += 16) {  // Loop through screen height
			SDL_FRect destTile = { i, j, 16, 16 };  // Position for each tile
			SDL_RenderTexture(renderer, backgroundTexture, &srcTile, &destTile);
		}
	}

	SDL_FRect floorRect = { 0, 400, 800, 200 };  // Floor rectangle
	if (SDL_SetRenderDrawColor(renderer, 100, 50, 20, 255) == false) {
		std::cout << "fail render";
	}
	SDL_RenderFillRect(renderer, &floorRect);

	player->render(renderer);

	SDL_RenderPresent(renderer);
}

void Game::handleEvents() {
	if (SDL_PollEvent(&event) > 0) {
		if (event.type == SDL_EVENT_QUIT) {
			running = false;
		}


		if (event.type == SDL_EVENT_WINDOW_RESIZED) {
			int newWidth, newHeight;
			SDL_GetWindowSize(window, &newWidth, &newHeight);

			//  Maintain proper aspect ratio on resize
			SDL_SetRenderLogicalPresentation(renderer, newWidth, newHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);

			std::cout << "Window resized to: " << newWidth << "x" << newHeight << std::endl;
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

			if (key == SDLK_W && player->getIsJumping() == false) {
				std::cout << "w key\n";
				player->jump();
				
			}
			if (key == SDLK_S) player->setVelocity(player->getdx(), player->getSpeed());   // Move Down

			if (key == SDLK_A) {
				player->setVelocity(-player->getSpeed(), player->getdy());  // Move Left
			}
			if (key == SDLK_D) {
				player->setVelocity(player->getSpeed(), player->getdy());  // Move Right
			}
		}
		if (event.type == SDL_EVENT_KEY_UP) {
			SDL_Keycode key = event.key.key;
			if (key == SDLK_A && player->getdx() < 0) {
				player->setVelocity(0, player->getdy());
			}
			if (key == SDLK_D && player->getdx() > 0) {
				player->setVelocity(0, player->getdy());
			}
				
			// If the key released is related to vertical movement, stop dy
			if (key == SDLK_W && player->getdy() < 0) {
				player->setVelocity(player->getdx(), 0);
			}
			if (key == SDLK_S && player->getdy() > 0) {
				player->setVelocity(player->getdx(), 0);
			}

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

