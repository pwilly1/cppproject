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

void EnemyNPC::chaseTarget(GameObject* target) {
    float targetX = target->getX();

    if (targetX < getX()) {
        setdx(-getSpeed() / 2);  // chase left
    }
    else if (targetX > getX()) {
        setdx(getSpeed() / 2);   // chase right
    }
    else {
        setdx(0);  // stop if already aligned
    }
}
