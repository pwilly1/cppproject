#include "GameObject.h"
#include <iostream>

GameObject::GameObject(int x, int y, int w, int h, SDL_Renderer* renderer, const std::string& imagePath){
		this->x = x;
		this->y = y;
		this->width = w;
		this->height = h;
		this->dx = 0; // Objects start with no movement
		this->dy = 0;
		this-> texture = nullptr;

		texture = IMG_LoadTexture(renderer, imagePath.c_str());
		if (!texture) {
			std::cout << "Failed to load image: " << std::endl;
		}
		
}

GameObject::~GameObject() {
	if (texture) {
		SDL_DestroyTexture(texture);
	}

}

void GameObject::update(float deltaTime) {


}
void GameObject::render(SDL_Renderer* renderer) {
	if (texture) {
		SDL_FRect dstRect = { static_cast<float>(x), static_cast<float>(y),
							  static_cast<float>(width), static_cast<float>(height) };
		SDL_RenderTexture(renderer, texture, NULL, &dstRect);
	}
}

void GameObject::setVelocity(int dx, int dy) {
	this->dx = dx;
	this->dy = dy;
}
