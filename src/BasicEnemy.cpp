#include "BasicEnemy.h"

BasicEnemy::BasicEnemy(int x, int y, SDL_Renderer* renderer, const std::string& imagePath, GameObject* target)
    : EnemyNPC(x, y, renderer, imagePath) {
    this->target = target;
}



void BasicEnemy::update(float deltaTime) {

    if (!isAlive()) {
        state = EnemyState::Dead;
        return; 
    }
    switch (state) {
    case EnemyState::Idle:
        setdx(0);
        break;

    case EnemyState::Patrol:
        if (getX() <= patrolLeft) direction = 1;
        if (getX() + getWidth() >= patrolRight) direction = -1;
        setdx(direction * (getSpeed() / 2));
        break;

    case EnemyState::Chase:
        chaseTarget(target);
        break;

    case EnemyState::Attack:
        setdx(0);  // Placeholder
        break;

    case EnemyState::Dead:
        
        setdx(0);  // Maybe fade out or stop updating
        return;    // Exit early
    }


    applyPhysics(deltaTime);

    destRect.x = getX();
    destRect.y = getY();
}
