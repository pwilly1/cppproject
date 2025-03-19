#include <SDL3/SDL.h>
#include <iostream>
#include "Game.h"


Game::Game() {
	renderer = nullptr;
	window = nullptr;
	running = false;
}

bool Game::init() {
	running = true;

	SDL_Init(SDL_INIT_VIDEO);
	
	window = SDL_CreateWindow("My Game", 800, 800, SDL_WINDOW_RESIZABLE);
	if (!window) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;  // Exit if window creation fails
	}
	renderer = SDL_CreateRenderer(window, NULL);
	screenWidth = 800;
	screenHeight = 800;
	SDL_SetRenderLogicalPresentation(renderer, 800, 800, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

	world = new World(renderer);
	world->loadFromTMX("../../../resources/map.tmx");


	player = new GameObject(600, 450, renderer, "../../../resources/Heroes/Man/Naked/Idle.png");

	return true;
}

Game::~Game() {
	cleanup();  // Ensure proper cleanup when the game object is destroyed
}



void Game::render() {

	SDL_SetRenderDrawColor(renderer, 60, 34, 15, 255);
	SDL_RenderClear(renderer);

	world->render(renderer, cameraX, cameraY, screenWidth, screenHeight);


	player->render(renderer);

	SDL_RenderPresent(renderer);
}

void Game::handleEvents() {
	if (SDL_PollEvent(&event) > 0) {
		if (event.type == SDL_EVENT_QUIT) {
			running = false;
		}


		if (event.type == SDL_EVENT_WINDOW_RESIZED) {
			
			SDL_GetWindowSize(window, &screenWidth, &screenHeight);

			//  Maintain proper aspect ratio on resize
			SDL_SetRenderLogicalPresentation(renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

			std::cout << "Window resized to: " << screenWidth << "x" << screenHeight << std::endl;
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
	delete world;
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
		update(deltaTime);
		render();
		
	}
	cleanup();	
}

void Game::update(float deltaTime) {
	// Predict the player's next position
	float newX = player->getX() + player->getdx() * deltaTime + 0.5f;
	float newY = player->getY() + player->getdy() * deltaTime + 0.5f;

	// Convert position to tile-based coordinates
	int tileSize = 16;
	int leftTile = newX / tileSize;
	int rightTile = (newX + player->getPlayerWidth() - 1) / tileSize;
	int topTile = newY / tileSize;
	int bottomTile = (newY + player->getPlayerHeight() - 1) / tileSize;

	// Check for horizontal collision separately
	bool collisionX =
		world->isCollidable(leftTile, topTile) ||
		world->isCollidable(rightTile, topTile) ||
		world->isCollidable(leftTile, bottomTile) ||
		world->isCollidable(rightTile, bottomTile);

	// Apply movement only if no collision
	if (!collisionX) {
		player->setX(newX);
	}
	else {
		// Resolve horizontal collision
		if (player->getdx() > 0) { // Moving Right, push left
			player->setX((rightTile * tileSize) - player->getPlayerWidth());
		}
		else if (player->getdx() < 0) { // Moving Left, push right
			player->setX((leftTile + 1) * tileSize);
		}
	}

	// Recalculate tile positions with the adjusted X position
	leftTile = player->getX() / tileSize;
	rightTile = (player->getX() + player->getPlayerWidth() - 1) / tileSize;

	// Check for vertical collision separately
	bool collisionY =
		world->isCollidable(leftTile, topTile) ||
		world->isCollidable(rightTile, topTile) ||
		world->isCollidable(leftTile, bottomTile) ||
		world->isCollidable(rightTile, bottomTile);

	if (!collisionY) {
		player->setY(newY);
	}
	else {
		// Resolve vertical collision
		if (player->getdy() > 0) { // Moving Down, push up
			player->setY((bottomTile * tileSize) - player->getPlayerHeight());
		}
		else if (player->getdy() < 0) { // Moving Up, push down
			player->setY((topTile + 1) * tileSize);
		}
	}

	// Final position update
	player->update(deltaTime);

	// Move the camera based on the player's position
	cameraX = player->getX() + (player->getPlayerWidth() / 2) - (800 / 2);
	cameraY = player->getY() + (player->getPlayerHeight() / 2) - (800 / 2);
}




