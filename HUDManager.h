#ifndef HUDMANAGER_H
#define HUDMANAGER_H
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>



class HUDManager {
public:
	HUDManager(TTF_TextEngine* engine, SDL_Renderer* renderer, float x, float y);
	void render(SDL_Renderer* renderer, int stoneCollected);

private:
	TTF_Font* font;
	TTF_TextEngine* engine;
	SDL_Texture* texture;
	float x;
	float y;

};

#endif