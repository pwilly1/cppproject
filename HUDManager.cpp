#include "HUDManager.h"
#include <iostream>
#include <string>

HUDManager::HUDManager(TTF_TextEngine* engine, SDL_Renderer* renderer, float x, float y) {

	this->font = TTF_OpenFont("../../../resources/Fonts/Heebo/static/Heebo-Regular.ttf", 20);
	if (!font) {
		std::cout << "font invalid" << SDL_GetError();
	}
	this->engine = engine;
	this->texture = nullptr;
	this->x = x;
	this->y = y;
}

void HUDManager::render(SDL_Renderer* renderer, int stoneCollected) {
	std::string stone = std::to_string(stoneCollected);
	char const* pchar = stone.c_str();  //use char const* as target type
	char const* inv = "40";
	int length = stone.length();
	TTF_Text* text = TTF_CreateText(engine, font, pchar, length);
	if (!text) {
		std::cout << "text is null" << SDL_GetError();
	}

	if (!TTF_DrawRendererText(text, 10, 10)) {
		std::cout << "cant render text" << SDL_GetError() << std::endl;
	}


}












