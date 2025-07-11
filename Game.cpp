#include <SDL3/SDL.h>
#include <iostream>
#include "Game.h"
#include "Player.h"
#include "BasicEnemy.h"

Game::Game() {
	renderer = nullptr;
	window = nullptr;
	running = false;
}

bool Game::init() {
	running = true;

	TTF_Init();
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(displayID);

	if (mode) {
		screenWidth = mode->w;
		screenHeight = mode->h;
		std::cout << "Screen size: " << screenWidth << " x " << screenHeight << std::endl;
	}
	else {
		std::cout << "Failed to get display mode: " << SDL_GetError() << std::endl;
	}

	int widthscreen = screenWidth;
	int heightscreen = screenHeight;


	window = SDL_CreateWindow("My Game", 800, 800, SDL_WINDOW_RESIZABLE);
	if (!window) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;  // Exit if window creation fails
	}
	renderer = SDL_CreateRenderer(window, NULL);
	textEngine = TTF_CreateRendererTextEngine(renderer);
	
	SDL_SetRenderLogicalPresentation(renderer, 800, 800, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

	inventory = new Inventory();

	HUD = new HUDManager(textEngine, renderer, HUDLocationX, HUDLocationY, inventory, screenWidth, screenHeight);

	world = new World(renderer);
	world->loadFromTMX("../../../resources/map.tmx");


	player = new Player(550, 1030, renderer, "../../../resources/Heroes/Man/Naked/idle.png", inventory);
	enemy = new BasicEnemy(700, 1020, renderer, "../../../resources/Heroes/Knight/Idle/Idle-Sheet.png", player);

	return true;
}

Game::~Game() {
	cleanup(); 
}



void Game::render() {

	SDL_SetRenderDrawColor(renderer, 60, 34, 15, 255);
	SDL_RenderClear(renderer);

	world->render(renderer, cameraX, cameraY, screenWidth, screenHeight);

	enemy->render(renderer, cameraX, cameraY);
	player->render(renderer, cameraX, cameraY);
	HUD->render(renderer);

	SDL_RenderPresent(renderer);

}

void Game::handleEvents() {

	//check for events
	if (SDL_PollEvent(&event) > 0) {
		if (event.type == SDL_EVENT_QUIT) {
			running = false;
		}


		if (event.type == SDL_EVENT_WINDOW_RESIZED) {
			
			SDL_GetWindowSize(window, &screenWidth, &screenHeight);

			//Maintain proper aspect ratio on resize
			SDL_SetRenderLogicalPresentation(renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

			std::cout << "Window resized to: " << screenWidth << "x" << screenHeight << std::endl;
		}
	

		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			SDL_MouseButtonFlags button = event.button.button;
			//float mouseX;
			//float mouseY;
			//SDL_GetMouseState(&mouseX, &mouseY);
			//Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
			//SDL_BUTTON_LMASK is left mouse button mask that checks if left mouse button is being held down
			if (button == SDL_BUTTON_LEFT) {
				std::cout << "left mouse pressed\n";
				//std::cout << "mouse x: " << mouseX << "mouseY: " << mouseY << std::endl;
				//mouseX += cameraX;
				//mouseY += cameraY;
				//world->breakTile(mouseX, mouseY);
			}
			if (button == SDL_BUTTON_MIDDLE) {
				std::cout << "middle mousepressed\n";
			}
			if (button == SDL_BUTTON_RIGHT) {
				std::cout << "right mouse pressed\n";
			}
		}




		//keyboard events
		if (event.type == SDL_EVENT_KEY_DOWN) {

			SDL_Keycode key = event.key.key;

			if (key >= SDLK_1 && key <= SDLK_9) {
				int index = key - SDLK_1; 
				std::cout << "index: " <<  index << std::endl;
				inventory->setSelectedIndex(index);
				if (inventory->getItem().canBreak()) {
					player->toggleBreakMode();
				}
				else {
					player->setBreakMode(false);
				}
				if (inventory->getItem().canPlace()) {
					player->setPlaceMode(true);
				}
				else {
					player->setPlaceMode(false);
				}

			}
			else if (key == SDLK_0) {
				inventory->setSelectedIndex(9);  // 0 key  index 9
				if (inventory->getItem().canBreak()) {
					player->toggleBreakMode();
				}
				else{
					player->setBreakMode(false);
				}
				if (inventory->getItem().canPlace()) {
					player->setPlaceMode(true);
				}
				else {
					player->setPlaceMode(false);
				}
			}

			if (key == SDLK_W) {
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
			if (key == SDLK_1) {
				std::cout << "Break mode toggled" << std::endl;
				//player->toggleBreakMode();
			}
		}

		//stop player movement when key is released
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



void Game::run() {
	Uint64 lastTime = SDL_GetTicks();

	while (running) {
		Uint64 currentTime= SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;

		//Had to add this because resizing screen Halts game loop
		if (deltaTime > 0.05f) {
			deltaTime = 0.05f;
		}


		update(deltaTime);
		world->checkWallCollisons(*enemy, cameraX, cameraY);
		world->checkWallCollisons(*player, cameraX, cameraY);
		handleEvents();
		render();
		
	}
	cleanup();	
}

void Game::update(float deltaTime) {
	
	player->update(deltaTime);
	enemy->update(deltaTime);

	// Check for mouse input to break tiles
	float mouseX, mouseY;
	Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
	
	/*if (inventory->getInventory()[inventory->getSelectedIndex()].name.empty()) {
		player->setBreakMode(false);
		player->setPlaceMode(false);
	}
	*/
	if (buttons & SDL_BUTTON_LMASK) {
		float worldX = mouseX / Gzoom + cameraX;
		float worldY = mouseY / Gzoom + cameraY;
		if (player->getBreakMode()) {
		world->breakTile(worldX, worldY, inventory);
	}
		else if (player->getPlaceMode()) {
			world->placeTile(worldX, worldY, inventory);
			player->setPlaceMode(false);
		}
	}


	// Move the camera based on the player's position
	// The camera is centered on the player, adjusting for the screen size

	float targetX = player->getX() + player->getWidth() / 2 - screenWidth / (2 * Gzoom);
	float targetY = player->getY() + player->getHeight() / 2 - screenHeight / (2 * Gzoom);

	// interpolate
	cameraX += (targetX - cameraX) * 0.1f;
	cameraY += (targetY - cameraY) * 0.1f;

	// snap to integers before rendering
	cameraX = floor(cameraX);
	cameraY = floor(cameraY);

	

}


void Game::cleanup() {
	if (player) {
		delete player;
		player = nullptr;
	}

	if (world) {
		delete world;
		world = nullptr;
	}

	if (HUD) {
		delete HUD;
		HUD = nullptr;
	}

	if (inventory) {
		delete inventory;
		inventory = nullptr;
	}

	if (textEngine) {
		TTF_DestroyRendererTextEngine(textEngine);
		textEngine = nullptr;
	}

	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}

	if (window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	TTF_Quit();
	SDL_Quit();
}



