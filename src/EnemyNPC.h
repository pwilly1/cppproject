// EnemyNPC.h
#ifndef ENEMYNPC_H
#define ENEMYNPC_H

#include "GameObject.h"




class EnemyNPC : public GameObject {
public:
    enum class EnemyState {
        Idle,
        Patrol,
        Chase,
        Attack,
        Dead
    };

    EnemyNPC(int x, int y, SDL_Renderer* renderer, const std::string& imagePath);
    virtual ~EnemyNPC() {}

    virtual void update(float deltaTime) override = 0; // make abstract
    void takeDamage(int dmg);
    bool isAlive() const;

    void chaseTarget(GameObject* target);


protected:

};

#endif
