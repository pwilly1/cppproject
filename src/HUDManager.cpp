#include "HUDManager.h"
#include "Player.h"
#include <iostream>
#include <string>
#include <SDL3_image/SDL_image.h>
#include "Inventory.h"

HUDManager::HUDManager(TTF_TextEngine* engine, SDL_Renderer* renderer, float x, float y, Inventory* inventory, int screenWidth, int screenHeight) {



	srcBox = { 0, 0, 60, 60 };
	inventoryBoxTexture = IMG_LoadTexture(renderer, "resources/Elements/3-1.png");
	font = TTF_OpenFont("resources/Fonts/Heebo/static/Heebo-Regular.ttf", 20);
	if (!font) {
		std::cout << "font invalid" << SDL_GetError();
	}
	countFont = TTF_OpenFont("resources/Fonts/Heebo/static/Heebo-Regular.ttf", 12);
	if (!countFont) {
		std::cout << "countFont invalid" << SDL_GetError();
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

	// Health bar - top left
	if (player) {
		int hp = player->getHealth();
		if (hp < 0) hp = 0;
		const float barX = 10.0f, barY = 10.0f;
		const float barW = 130.0f, barH = 16.0f;
		float fill = hp / 100.0f;

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		// Background
		SDL_SetRenderDrawColor(renderer, 60, 0, 0, 200);
		SDL_FRect bgRect = { barX, barY, barW, barH };
		SDL_RenderFillRect(renderer, &bgRect);

		// Fill — green above 50%, yellow 25-50%, red below 25%
		if (hp > 50)
			SDL_SetRenderDrawColor(renderer, 50, 180, 50, 255);
		else if (hp > 25)
			SDL_SetRenderDrawColor(renderer, 210, 180, 40, 255);
		else
			SDL_SetRenderDrawColor(renderer, 210, 45, 45, 255);
		SDL_FRect fillRect = { barX, barY, barW * fill, barH };
		SDL_RenderFillRect(renderer, &fillRect);

		// Border
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 120);
		SDL_RenderRect(renderer, &bgRect);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

		// HP text
		if (countFont) {
			std::string hpStr = "HP  " + std::to_string(hp) + " / 100";
			TTF_Text* hpText = TTF_CreateText(engine, countFont, hpStr.c_str(), 0);
			if (hpText) {
				TTF_SetTextColor(hpText, 255, 255, 255, 255);
				TTF_DrawRendererText(hpText, barX + 3.0f, barY + 2.0f);
				TTF_DestroyText(hpText);
			}
		}
	}

	float boxX = (screenWidth / 2) - 200;
	float boxY = screenHeight - 100;

	// Draw slot backgrounds
	for (int i = 0; i < 10; i++) {
		destBox = { boxX + static_cast<float>(i * 40), boxY, 30, 30 };
		SDL_RenderTexture(renderer, inventoryBoxTexture, &srcBox, &destBox);
	}

	// Draw item icons and stack counts
	int itemX = 0;
	for (auto& inventoryItem : inventory->getInventory()) {
		if (itemX >= 10) break;

		float slotX = boxX + itemX * 40;

		if (!inventoryItem.name.empty()) {
			// Draw item icon
			SDL_Texture* itemTex = IMG_LoadTexture(renderer, inventoryItem.filename.c_str());
			if (itemTex) {
				SDL_FRect srcRect = { 0, 0, 64, 64 };
				SDL_FRect destRect = { slotX, boxY, 25, 25 };
				SDL_RenderTexture(renderer, itemTex, &srcRect, &destRect);
				SDL_DestroyTexture(itemTex);
			}

			// Draw stack count in bottom-right of slot
			if (inventoryItem.amount > 1 && countFont) {
				std::string countStr = std::to_string(inventoryItem.amount);
				TTF_Text* countText = TTF_CreateText(engine, countFont, countStr.c_str(), 0);
				if (countText) {
					int tw, th;
					TTF_GetTextSize(countText, &tw, &th);
					TTF_SetTextColor(countText, 255, 255, 255, 255);
					TTF_DrawRendererText(countText, slotX + 30 - tw, boxY + 30 - th);
					TTF_DestroyText(countText);
				}
			}
		}

		itemX++;
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
	if (countFont) {
		TTF_CloseFont(countFont);
		countFont = nullptr;
	}
}












