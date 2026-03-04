#include <SDL3/SDL.h>
#include <iostream>
#include <filesystem>
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
	logW = 800;
	logH = 800;
	updateButtonLayouts();
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
		renderMenu();
	} else if (currentState == GameState::Paused) {
		renderPaused();
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
			logW = screenWidth;
			logH = screenHeight;
			updateButtonLayouts();
			std::cout << "Window resized to: " << screenWidth << "x" << screenHeight << std::endl;
		}

		// Hover detection for menu and pause buttons
		if (event.type == SDL_EVENT_MOUSE_MOTION) {
			float lx, ly;
			SDL_RenderCoordinatesFromWindow(renderer, event.motion.x, event.motion.y, &lx, &ly);
			auto inRect = [](const SDL_FRect& r, float x, float y) {
				return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
			};
			if (currentState == GameState::Menu) {
				btnNewGame.hovered  = inRect(btnNewGame.rect,  lx, ly);
				btnLoadGame.hovered = inRect(btnLoadGame.rect, lx, ly) && btnLoadGame.enabled;
			} else if (currentState == GameState::Paused) {
				btnResume.hovered   = inRect(btnResume.rect,   lx, ly);
				btnSave.hovered     = inRect(btnSave.rect,     lx, ly);
				btnMainMenu.hovered = inRect(btnMainMenu.rect, lx, ly);
				btnQuit.hovered     = inRect(btnQuit.rect,     lx, ly);
			}
		}

		// Button clicks for menu and pause
		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
			float lx, ly;
			SDL_RenderCoordinatesFromWindow(renderer, event.button.x, event.button.y, &lx, &ly);
			auto inRect = [](const SDL_FRect& r, float x, float y) {
				return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
			};
			if (currentState == GameState::Menu) {
				if (inRect(btnNewGame.rect, lx, ly)) {
					reset();
					currentState = GameState::Playing;
				} else if (inRect(btnLoadGame.rect, lx, ly) && btnLoadGame.enabled) {
					loadGame();
					currentState = GameState::Playing;
				}
			} else if (currentState == GameState::Paused) {
				if      (inRect(btnResume.rect,   lx, ly)) { currentState = GameState::Playing; }
				else if (inRect(btnSave.rect,     lx, ly)) { saveGame(); }
				else if (inRect(btnMainMenu.rect, lx, ly)) { currentState = GameState::Menu; }
				else if (inRect(btnQuit.rect,     lx, ly)) { running = false; }
			}
		}

		if (event.type == SDL_EVENT_KEY_DOWN) {
			SDL_Keycode key = event.key.key;

			if (currentState == GameState::Menu) {
				if (key == SDLK_RETURN) { reset(); currentState = GameState::Playing; }
			} else if (currentState == GameState::Paused) {
				if (key == SDLK_ESCAPE) currentState = GameState::Playing;
				if (key == SDLK_S) saveGame();
				if (key == SDLK_M) currentState = GameState::Menu;
				if (key == SDLK_Q) running = false;
			} else if (currentState == GameState::GameOver) {
				if (key == SDLK_RETURN) { reset(); currentState = GameState::Playing; }
			} else if (currentState == GameState::Playing) {
				if (key == SDLK_ESCAPE) currentState = GameState::Paused;
				if (key == SDLK_F5) saveGame();
				if (key == SDLK_F9) loadGame();

				if (key >= SDLK_1 && key <= SDLK_9) {
					int index = key - SDLK_1;
					std::cout << "index: " << index << std::endl;
					inventory->setSelectedIndex(index);
					if (inventory->getItem().canBreak()) { player->toggleBreakMode(); }
					else { player->setBreakMode(false); }
					if (inventory->getItem().canPlace()) { player->setPlaceMode(true); }
					else { player->setPlaceMode(false); }
				} else if (key == SDLK_0) {
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
	player = nullptr;
	delete enemy;
	enemy = nullptr;

	inventory->clear();

	world->generateMap(300, 300);
	world->loadFromTMX("resources/map.tmx");

	player = new Player(1000, 700, renderer, "resources/Heroes/Man/Naked/idle.png", inventory);
	enemy = new BasicEnemy(1020, 700, renderer, "resources/Heroes/Knight/Idle/Idle-Sheet.png", player);

	cameraX = 0;
	cameraY = 0;
}

void Game::updateButtonLayouts() {
	const float bw = 200.0f, bh = 45.0f;
	float cx = logW / 2.0f;
	float cy = logH / 2.0f;

	btnNewGame  = { {cx - bw / 2, cy - 10,  bw, bh}, "New Game" };
	btnLoadGame = { {cx - bw / 2, cy + 55,  bw, bh}, "Load Game" };

	btnResume   = { {cx - bw / 2, cy - 100, bw, bh}, "Resume" };
	btnSave     = { {cx - bw / 2, cy - 45,  bw, bh}, "Save" };
	btnMainMenu = { {cx - bw / 2, cy + 10,  bw, bh}, "Main Menu" };
	btnQuit     = { {cx - bw / 2, cy + 65,  bw, bh}, "Quit" };
}

bool Game::saveFileExists() const {
	return std::filesystem::exists("save.xml");
}

void Game::renderButton(const Button& btn) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	if (!btn.enabled) {
		SDL_SetRenderDrawColor(renderer, 40, 40, 40, 180);
	} else if (btn.hovered) {
		SDL_SetRenderDrawColor(renderer, 80, 100, 180, 220);
	} else {
		SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
	}
	SDL_RenderFillRect(renderer, &btn.rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	SDL_SetRenderDrawColor(renderer, 200, 200, 200, btn.enabled ? 255 : 80);
	SDL_RenderRect(renderer, &btn.rect);

	if (stateFontSmall) {
		TTF_Text* txt = TTF_CreateText(textEngine, stateFontSmall, btn.label.c_str(), 0);
		if (txt) {
			int tw, th;
			TTF_GetTextSize(txt, &tw, &th);
			TTF_SetTextColor(txt, 255, 255, 255, btn.enabled ? 255 : 80);
			TTF_DrawRendererText(txt,
				btn.rect.x + (btn.rect.w - tw) / 2.0f,
				btn.rect.y + (btn.rect.h - th) / 2.0f);
			TTF_DestroyText(txt);
		}
	}
}

void Game::renderMenu() {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
	SDL_FRect overlay = { 0, 0, static_cast<float>(logW), static_cast<float>(logH) };
	SDL_RenderFillRect(renderer, &overlay);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	if (stateFont) {
		TTF_Text* title = TTF_CreateText(textEngine, stateFont, "My Game", 0);
		if (title) {
			int w, h;
			TTF_GetTextSize(title, &w, &h);
			TTF_SetTextColor(title, 255, 255, 255, 255);
			TTF_DrawRendererText(title, (logW - w) / 2.0f, logH / 2.0f - 130);
			TTF_DestroyText(title);
		}
	}

	btnLoadGame.enabled = saveFileExists();
	renderButton(btnNewGame);
	renderButton(btnLoadGame);
}

void Game::renderPaused() {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
	SDL_FRect overlay = { 0, 0, static_cast<float>(logW), static_cast<float>(logH) };
	SDL_RenderFillRect(renderer, &overlay);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	if (stateFont) {
		TTF_Text* title = TTF_CreateText(textEngine, stateFont, "Paused", 0);
		if (title) {
			int w, h;
			TTF_GetTextSize(title, &w, &h);
			TTF_SetTextColor(title, 255, 255, 255, 255);
			TTF_DrawRendererText(title, (logW - w) / 2.0f, logH / 2.0f - 160);
			TTF_DestroyText(title);
		}
	}

	renderButton(btnResume);
	renderButton(btnSave);
	renderButton(btnMainMenu);
	renderButton(btnQuit);
}

void Game::saveGame() {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* root = doc.NewElement("save");
	doc.InsertFirstChild(root);

	tinyxml2::XMLElement* playerEl = doc.NewElement("player");
	playerEl->SetAttribute("x", player->getX());
	playerEl->SetAttribute("y", player->getY());
	playerEl->SetAttribute("health", player->getHealth());
	root->InsertEndChild(playerEl);

	inventory->saveToXML(doc, root);
	world->saveToXML(doc, root);

	if (doc.SaveFile("save.xml") == tinyxml2::XML_SUCCESS) {
		std::cout << "Game saved.\n";
	} else {
		std::cout << "Failed to save game.\n";
	}
}

void Game::loadGame() {
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile("save.xml") != tinyxml2::XML_SUCCESS) {
		std::cout << "No save file found.\n";
		return;
	}

	tinyxml2::XMLElement* root = doc.FirstChildElement("save");
	if (!root) return;

	tinyxml2::XMLElement* playerEl = root->FirstChildElement("player");
	if (playerEl) {
		player->setX(playerEl->FloatAttribute("x"));
		player->setY(playerEl->FloatAttribute("y"));
		player->setHealth(playerEl->IntAttribute("health"));
	}

	inventory->loadFromXML(root->FirstChildElement("inventory"));
	world->loadFromXML(root->FirstChildElement("world"));

	std::cout << "Game loaded.\n";
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



