#ifndef HUDMANAGER_H
#define HUDMANAGER_H
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
class Inventory;


class HUDManager {
public:
	HUDManager(TTF_TextEngine* engine, SDL_Renderer* renderer, float x, float y, Inventory* inventory, int screenWidth, int screenHeight);
	~HUDManager();
	void render(SDL_Renderer* renderer);

private:
	Inventory* inventory;
	SDL_FRect srcBox;
	SDL_FRect destBox;
	SDL_Texture* inventoryBoxTexture;
	TTF_Font* font;
	TTF_TextEngine* engine;
	SDL_Texture* inventoryItemTexture;
	float x;
	float y;
	int screenWidth;
	int screenHeight;

};

#endif