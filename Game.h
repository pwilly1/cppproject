#ifndef GAME_H
#define GAME_H
#include "GameObject.h"
#include "World.h" 
#include <SDL3/SDL.h>
#include <iostream>
#include <SDL3_ttf/SDL_ttf.h>
#include "HUDManager.h"
#include "Inventory.h"

class BasicEnemy;
class Player;
class World;
class Game {
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
	bool running = false;



};

#endif