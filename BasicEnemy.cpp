#include "BasicEnemy.h"

BasicEnemy::BasicEnemy(int x, int y, SDL_Renderer* renderer, const std::string& imagePath)
    : EnemyNPC(x, y, renderer, imagePath),
    patrolLeft(x - 100), patrolRight(x + 100) {
}

void BasicEnemy::update(float deltaTime) {
    if (getX() <= patrolLeft) direction = 1;
    if (getX() + getWidth() >= patrolRight) direction = -1;

    setdx(direction * (getSpeed()/ 2));
    applyPhysics(deltaTime);

    destRect.x = getX();
    destRect.y = getY();
}
