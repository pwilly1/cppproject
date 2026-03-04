#ifndef GAME_H
#define GAME_H
#include "GameObject.h"
#include "World.h" 
#include <SDL3/SDL.h>
#include <iostream>
#include <SDL3_ttf/SDL_ttf.h>
#include "HUDManager.h"
#include "Inventory.h"

enum class GameState {
	Menu,
	Playing,
	Paused,
	GameOver
};

class BasicEnemy;
class Player;
class World;
class Game {
	struct Button {
		SDL_FRect rect;
		std::string label;
		bool hovered = false;
		bool enabled = true;
	};
public:

	Game();
	~Game();

	bool init();
	void render();
	void handleEvents();
	void run();
	void cleanup();
	void update(float deltaTime);

private:
	void renderOverlay(const std::string& title, const std::string& subtitle);
	void renderMenu();
	void renderPaused();
	void renderButton(const Button& btn);
	void updateButtonLayouts();
	bool saveFileExists() const;
	void reset();
	void saveGame();
	void loadGame();
	float HUDLocationX = 10;
	float HUDLocationY = 10;
	BasicEnemy* enemy;
	float Gzoom = 2;
	Inventory* inventory;
	HUDManager* HUD;
	World* world;
	float cameraX = 0;
	float cameraY = 0;
	int screenWidth;
	int screenHeight;
	int winw, winh;
	SDL_Texture* backgroundTexture;
	Player* player;
	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window = nullptr; //window pointer object that points to window created.
	TTF_TextEngine* textEngine;
	TTF_Font* stateFont = nullptr;
	TTF_Font* stateFontSmall = nullptr;
	GameState currentState = GameState::Menu;
	int logW = 800, logH = 800;
	Button btnNewGame, btnLoadGame;
	Button btnResume, btnSave, btnMainMenu, btnQuit;
	bool running = false;



};

#endif