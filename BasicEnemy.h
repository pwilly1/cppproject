#ifndef BASICENEMY_H
#define BASICENEMY_H

#include "EnemyNPC.h"

class BasicEnemy : public EnemyNPC {
public:
    BasicEnemy(int x, int y, SDL_Renderer* renderer, const std::string& imagePath);

    void update(float deltaTime) override;

private:
    int direction = 1;
    float patrolLeft;
    float patrolRight;
};

#endif
