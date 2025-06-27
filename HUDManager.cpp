#include "HUDManager.h"
#include <iostream>
#include <string>
#include <SDL3_image/SDL_image.h>

HUDManager::HUDManager(TTF_TextEngine* engine, SDL_Renderer* renderer, float x, float y, Inventory* inventory, int screenWidth, int screenHeight) {



	srcBox = { 0, 0, 60, 60 };
	inventoryBoxTexture = IMG_LoadTexture(renderer, "../../../resources/Elements/3-1.png");
	font = TTF_OpenFont("../../../resources/Fonts/Heebo/static/Heebo-Regular.ttf", 20);
	if (!font) {
		std::cout << "font invalid" << SDL_GetError();
	}
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
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
	float boxX = (screenWidth / 2) - 200;
	float boxY = screenHeight - 80;
	for (int i = 0; i < 10; i++) {
		int x = i;
		x *= 40;
		destBox = { boxX + static_cast<float>(x), boxY, 30, 30 };

		SDL_RenderTexture(renderer, inventoryBoxTexture, &srcBox, &destBox);
	}


}












