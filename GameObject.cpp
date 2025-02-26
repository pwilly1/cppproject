#include "GameObject.h"
#include <iostream>

GameObject::GameObject(int x, int y, SDL_Renderer* renderer, const std::string& imagePath) {
    std::cout << "GameObject constructor called!" << std::endl;

    this->x = x;
    this->y = y;
    this->width = 0;
    this->height = 0;
    this->dx = 0;
    this->dy = 0;
    this->speed = 100;
    this->texture = nullptr;

    if (!renderer) {
        std::cout << "Renderer is NULL before loading texture!" << std::endl;
        return;
    }

    texture = IMG_LoadTexture(renderer, imagePath.c_str());
    if (!texture) {
        std::cout << "Failed to load image: " << SDL_GetError() << std::endl;
        return;
    }
    std::cout << "Object Texture loaded successfully!" << std::endl;


    float imageWidth, imageHeight;
    std::cout << "Querying Object texture size..." << std::endl;

    if (SDL_GetTextureSize(texture, &imageWidth, &imageHeight) != true) {
        std::cout << "Failed to get texture size: " << SDL_GetError() << std::endl;
        return;
    }

    std::cout << "Texture size: " << imageWidth << "x" << imageHeight << std::endl;

    float frameWidth = imageWidth / 4;
    float frameHeight = imageHeight;

    this->height = frameHeight;
    this->width = frameWidth;

    srcRect = { 0, 0, frameWidth, frameHeight };
    destRect = { static_cast<float>(x), static_cast<float>(y), frameWidth, frameHeight };
}


GameObject::~GameObject() {
	if (texture) {
		SDL_DestroyTexture(texture);
	}

}


void GameObject::SetGroundLevel() {

}

bool GameObject::isOnGround() {
    if (y >= groundLevel - height) {
        return true;
    }
    else {
        return false;
    }
}

void GameObject::update(float deltaTime) {


    dy += gravity * deltaTime;

    x += dx * deltaTime;
    y += dy * deltaTime;

    if (isOnGround() == true) {  // Prevents the object from sinking into the ground
        y = groundLevel - height;
        
        isJumping = false;
    }


    destRect.x = static_cast<int>(x);
    destRect.y = static_cast<int>(y);


}
void GameObject::render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderTexture(renderer, texture, &srcRect, &destRect);
    }
    else {
        std::cout << "Warning: Trying to render a NULL texture!" << std::endl;
    }
	
}

void GameObject::setVelocity(int dx, int ignoreDy) {
    this->dx = dx;  //  Only modify dx, not dy
}

bool GameObject::getIsJumping() {
    std::cout << isJumping;
    return isJumping;
}

void GameObject::jump() {
    if (isJumping == false && isOnGround() == true) {
        std::cout << "jumpiscALLED" << std::endl;
        dy = -jumpPower;
        isJumping = true;
    }
    
}

