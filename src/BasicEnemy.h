#ifndef BASICENEMY_H
#define BASICENEMY_H

#include "EnemyNPC.h"

class BasicEnemy : public EnemyNPC {
public:
    BasicEnemy(int x, int y, SDL_Renderer* renderer, const std::string& imagePath, GameObject* target);

    void update(float deltaTime);


private:
    EnemyState state = EnemyState::Chase;  // start chasing by default
    GameObject* target;
    int direction = 1;
    float patrolLeft = -1;
    float patrolRight = 1;
};

#endif
