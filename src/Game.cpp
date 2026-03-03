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

	if (!TTF_Init()) {
		std::cout << "TTF_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}
	
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
	world->loadFromTMX("resources/map.tmx");


	player = new Player(1000, 700, renderer, "resources/Heroes/Man/Naked/idle.png", inventory);
	enemy = new BasicEnemy(1020, 700, renderer, "resources/Heroes/Knight/Idle/Idle-Sheet.png", player);

	stateFont = TTF_OpenFont("resources/font.ttf", 48);
	if (!stateFont) {
		stateFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 48);
	}
	stateFontSmall = TTF_OpenFont("resources/font.ttf", 24);
	if (!stateFontSmall) {
		stateFontSmall = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
	}

	currentState = GameState::Menu;
	return true;
}

Game::~Game() {
	cleanup(); 
}



void Game::render() {
	SDL_RenderClear(renderer);

	world->updateBackgroundForPlayer(player->getX(), player->getY());
	world->render(renderer, cameraX, cameraY, screenWidth, screenHeight);

	if (currentState != GameState::Menu) {
		if (enemy != nullptr && enemy->isAlive()) {
			enemy->render(renderer, cameraX, cameraY);
		}
		player->render(renderer, cameraX, cameraY);
		HUD->render(renderer);
	}

	if (currentState == GameState::Menu) {
		renderOverlay("My Game", "Press ENTER to Play");
	} else if (currentState == GameState::Paused) {
		renderOverlay("Paused", "ESC to Resume   |   Q to Quit");
	} else if (currentState == GameState::GameOver) {
		renderOverlay("Game Over", "Press ENTER to Restart");
	}

	SDL_RenderPresent(renderer);
}

void Game::handleEvents() {
	if (SDL_PollEvent(&event) > 0) {
		if (event.type == SDL_EVENT_QUIT) {
			running = false;
		}

		if (event.type == SDL_EVENT_WINDOW_RESIZED) {
			SDL_GetWindowSize(window, &screenWidth, &screenHeight);
			SDL_SetRenderLogicalPresentation(renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
			std::cout << "Window resized to: " << screenWidth << "x" << screenHeight << std::endl;
		}

		if (event.type == SDL_EVENT_KEY_DOWN) {
			SDL_Keycode key = event.key.key;

			if (currentState == GameState::Menu) {
				if (key == SDLK_RETURN) currentState = GameState::Playing;
			}
			else if (currentState == GameState::Paused) {
				if (key == SDLK_ESCAPE) currentState = GameState::Playing;
				if (key == SDLK_Q) running = false;
			}
			else if (currentState == GameState::GameOver) {
				if (key == SDLK_RETURN) { reset(); currentState = GameState::Playing; }
			}
			else if (currentState == GameState::Playing) {
				if (key == SDLK_ESCAPE) currentState = GameState::Paused;

				if (key >= SDLK_1 && key <= SDLK_9) {
					int index = key - SDLK_1;
					std::cout << "index: " << index << std::endl;
					inventory->setSelectedIndex(index);
					if (inventory->getItem().canBreak()) { player->toggleBreakMode(); }
					else { player->setBreakMode(false); }
					if (inventory->getItem().canPlace()) { player->setPlaceMode(true); }
					else { player->setPlaceMode(false); }
				}
				else if (key == SDLK_0) {
					inventory->setSelectedIndex(9);
					if (inventory->getItem().canBreak()) { player->toggleBreakMode(); }
					else { player->setBreakMode(false); }
					if (inventory->getItem().canPlace()) { player->setPlaceMode(true); }
					else { player->setPlaceMode(false); }
				}

				if (key == SDLK_W) player->jump();
				if (key == SDLK_S) player->setVelocity(player->getdx(), player->getSpeed());
				if (key == SDLK_A) player->setVelocity(-player->getSpeed(), player->getdy());
				if (key == SDLK_D) player->setVelocity(player->getSpeed(), player->getdy());
			}
		}

		if (currentState == GameState::Playing) {
			if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				SDL_MouseButtonFlags button = event.button.button;
				float mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				float worldX = mouseX / Gzoom + cameraX;
				float worldY = mouseY / Gzoom + cameraY;

				if (button == SDL_BUTTON_LEFT) {
					int tileX = static_cast<int>(worldX) / 16;
					int tileY = static_cast<int>(worldY) / 16;
					if (enemy != nullptr && enemy->isAlive()) {
						int enemyTileX = static_cast<int>(enemy->getX()) / 16;
						int enemyTileY = static_cast<int>(enemy->getY()) / 16;
						if (tileX == enemyTileX && tileY == enemyTileY) {
							enemy->takeDamage(50);
							std::cout << "enemy health reduced to " << enemy->getHealth() << std::endl;
						}
					}
				}
			}

			if (event.type == SDL_EVENT_KEY_UP) {
				SDL_Keycode key = event.key.key;
				if (key == SDLK_A && player->getdx() < 0) player->setVelocity(0, player->getdy());
				if (key == SDLK_D && player->getdx() > 0) player->setVelocity(0, player->getdy());
				if (key == SDLK_W && player->getdy() < 0) player->setVelocity(player->getdx(), 0);
				if (key == SDLK_S && player->getdy() > 0) player->setVelocity(player->getdx(), 0);
			}
		}
	}
}



void Game::run() {
	const float FIXED_TIMESTEP = 1.0f / 60.0f;
	float accumulator = 0.0f;
	Uint64 lastTime = SDL_GetTicks();

	while (running) {
		Uint64 currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;

		// Cap to prevent spiral of death after window resize stall
		if (deltaTime > 0.05f) {
			deltaTime = 0.05f;
		}

		handleEvents();

		accumulator += deltaTime;
		while (accumulator >= FIXED_TIMESTEP) {
			update(FIXED_TIMESTEP);
			if (enemy) {
				world->checkWallCollisions(*enemy, cameraX, cameraY);
			}
			if (player) {
				world->checkWallCollisions(*player, cameraX, cameraY);
			}
			accumulator -= FIXED_TIMESTEP;
		}

		render();
	}
	cleanup();	
}

void Game::update(float deltaTime) {
	if (currentState != GameState::Playing) return;

	player->update(deltaTime);
	if (enemy != nullptr) {
		enemy->update(deltaTime);

		// Check if enemy died
		if (!enemy->isAlive()) {
			delete enemy;
			enemy = nullptr;
		}
	}

	// Check for mouse input to break tiles
	float mouseX, mouseY;
	Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
	
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

	if (player->getHealth() <= 0) {
		currentState = GameState::GameOver;
	}
}


void Game::renderOverlay(const std::string& title, const std::string& subtitle) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
	SDL_FRect overlay = { 0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight) };
	SDL_RenderFillRect(renderer, &overlay);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	if (stateFont) {
		TTF_Text* titleText = TTF_CreateText(textEngine, stateFont, title.c_str(), 0);
		if (titleText) {
			int w, h;
			TTF_GetTextSize(titleText, &w, &h);
			TTF_SetTextColor(titleText, 255, 255, 255, 255);
			TTF_DrawRendererText(titleText, (screenWidth - w) / 2.0f, screenHeight / 2.0f - h - 10);
			TTF_DestroyText(titleText);
		}
	}

	if (stateFontSmall) {
		TTF_Text* subText = TTF_CreateText(textEngine, stateFontSmall, subtitle.c_str(), 0);
		if (subText) {
			int w, h;
			TTF_GetTextSize(subText, &w, &h);
			TTF_SetTextColor(subText, 200, 200, 200, 255);
			TTF_DrawRendererText(subText, (screenWidth - w) / 2.0f, screenHeight / 2.0f + 10);
			TTF_DestroyText(subText);
		}
	}
}

void Game::reset() {
	delete player;
	player = new Player(1000, 700, renderer, "resources/Heroes/Man/Naked/idle.png", inventory);

	delete enemy;
	enemy = new BasicEnemy(1020, 700, renderer, "resources/Heroes/Knight/Idle/Idle-Sheet.png", player);

	cameraX = 0;
	cameraY = 0;
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

	if (stateFont) {
		TTF_CloseFont(stateFont);
		stateFont = nullptr;
	}
	if (stateFontSmall) {
		TTF_CloseFont(stateFontSmall);
		stateFontSmall = nullptr;
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



