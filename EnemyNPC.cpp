// EnemyNPC.cpp
#include "EnemyNPC.h"

EnemyNPC::EnemyNPC(int x, int y, SDL_Renderer* renderer, const std::string& imagePath)
    : GameObject(x, y, renderer, imagePath) {
}

void EnemyNPC::takeDamage(int dmg) {
    health -= dmg;
    if (health < 0) health = 0;
}

bool EnemyNPC::isAlive() const {
    return health > 0;
}
