#include "Player.h"
#include "World.h"

Player::Player(int x, int y, SDL_Renderer* renderer, const std::string& imagePath, Inventory* inventory)
    : GameObject(x, y, renderer, imagePath), inventory(inventory) {
}

void Player::update(float deltaTime) {
    GameObject::applyPhysics(deltaTime);
    destRect.x = x;
    destRect.y = y;
}

void Player::toggleBreakMode() {
    breakMode = !breakMode;
}

void Player::setBreakMode(bool value) {
    breakMode = value;
}

bool Player::getBreakMode() const {
    return breakMode;
}

void Player::setPlaceMode(bool value) {
    placeMode = value;
}

bool Player::getPlaceMode() const {
    return placeMode;
}
