#include "HUDManager.h"
#include <iostream>
#include <string>
#include <SDL3_image/SDL_image.h>
#include "Inventory.h"

HUDManager::HUDManager(TTF_TextEngine* engine, SDL_Renderer* renderer, float x, float y, Inventory* inventory, int screenWidth, int screenHeight) {



	srcBox = { 0, 0, 60, 60 };
	inventoryBoxTexture = IMG_LoadTexture(renderer, "../../../resources/Elements/3-1.png");
	font = TTF_OpenFont("../../../resources/Fonts/Heebo/static/Heebo-Regular.ttf", 20);
	if (!font) {
		std::cout << "font invalid" << SDL_GetError();
	}
	this->inventory = inventory;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->engine = engine;
	this->inventoryItemTexture = nullptr;
	this->x = x;
	this->y = y;
}

void HUDManager::render(SDL_Renderer* renderer) {

	int stoneCollected = 40;

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
	float boxY = screenHeight - 100;

	for (int i = 0; i < 10; i++) {

		int x = i;
		x *= 40;
		destBox = { boxX + static_cast<float>(x), boxY, 30, 30 };

		SDL_RenderTexture(renderer, inventoryBoxTexture, &srcBox, &destBox);
	}
	 int itemX = 0;
		for (auto inventoryItem : inventory->getInventory()) {
			if (itemX < 10) {
				int destX = itemX * 40;

				inventoryItemTexture = IMG_LoadTexture(renderer, inventoryItem.filename.c_str());
				SDL_FRect srcRect = { 0, 0, 64, 64 };
				SDL_FRect destRect = { destX + boxX, boxY, 25, 25 };
				SDL_RenderTexture(renderer, inventoryItemTexture, &srcRect, &destRect);
				itemX++;
				//std::cout << inventoryItem.name << std::endl;
			}
			else {
				std::cout << "inventory full!" << std::endl;
			}
		}

}

HUDManager::~HUDManager() {

	if (inventoryBoxTexture) {
		SDL_DestroyTexture(inventoryBoxTexture);
		inventoryBoxTexture = nullptr;
	}
	if (font) {
		TTF_CloseFont(font);
		font = nullptr;
	}
}












