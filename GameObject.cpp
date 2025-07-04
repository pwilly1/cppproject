#include "GameObject.h"
#include "World.h"
#include <iostream>
#include "Inventory.h"

GameObject::GameObject(int x, int y, SDL_Renderer* renderer, const std::string& imagePath) {
    std::cout << "GameObject constructor called!" << std::endl;

    this->inventory = inventory;
    this->x = x;
    this->y = y;
    this->width = 0;
    this->height = 0;
    this->dx = 0;
    this->dy = 0;
    this->speed = 99;
    this->texture = nullptr;
    this->stoneCollected = 10;

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

    float frameWidth = imageWidth;
    float frameHeight = imageHeight;

    this->height = frameHeight;
    this->width = frameWidth;

	std::cout << "GameObject initialized at position: (" << x << ", " << y << ") with size: " << width << "x" << height << std::endl;

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


/*
bool GameObject::isOnGround() {
    if (y >= groundLevel - height) {
        return true;
    }
    else {
        return false;
    }
}
*/

void GameObject::update(float deltaTime, World& world) {


    dy += gravity * deltaTime;
    

    x += dx * deltaTime;
    y += dy * deltaTime;
   



    destRect.x = static_cast<int>(x);
    destRect.y = static_cast<int>(y);


}
void GameObject::render(SDL_Renderer* renderer, float cameraX, float cameraY) {

	// I subtracted cameraX and cameraY from the x and y coordinates to ensure that the object is rendered in world coordinates so it lines up with the map render.
	// It basically makes the camera the origin of the world, so the object is rendered relative to the camera position.
	// It does not change the actual x and y coordinates of the object, it just changes where it is rendered on the screen.

    float zoom = 2;
    if (texture) {
        SDL_FRect screenRect = {
            static_cast<float>(x - cameraX) * zoom, // this is the position of the player based on the screen not the world
            static_cast<float>(y - cameraY) * zoom, //it doesnt actually change the actual x and y of the object it just changes where its rendered on the screen
            static_cast<float>(width) * zoom,
            static_cast<float>(height) * zoom
        };
		// std::cout << "Rendering GameObject at: " << x << ", " << y << std::endl;
        SDL_RenderTexture(renderer, texture, &srcRect, &screenRect);
    }
    else {
        std::cout << "Warning: Trying to render a NULL texture!(GameObject)" << std::endl;
    }
	
}

void GameObject::setVelocity(int dx, int dy) {
    this->dx = dx;  //  Only modify dx, not dy
}

bool GameObject::getIsJumping() {
    std::cout << isJumping;
    return isJumping;
}

void GameObject::toggleBreakMode() {
    if(breakMode == false) {
        breakMode = true;
    }
    else {
        breakMode = false;
	}
}




void GameObject::jump(World& world) {
    if (world.isOnGround() == true) {
        std::cout << "jumpiscALLED" << std::endl;
        dy = -jumpPower;
        isJumping = true;
        world.setIsOnGround(false);
    }
    
}

