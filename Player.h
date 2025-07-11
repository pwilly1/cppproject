#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "Inventory.h"

class Player : public GameObject {
public:
    Player(int x, int y, SDL_Renderer* renderer, const std::string& imagePath, Inventory* inventory);

    void update(float deltaTime) override;

    void toggleBreakMode();
    void setBreakMode(bool value);
    bool getBreakMode() const;

    void setPlaceMode(bool value);
    bool getPlaceMode() const;

private:
    Inventory* inventory;
    bool isJumping = false;
    bool breakMode = false;
    bool placeMode = false;
    float jumpPower = 300.0f;
};

#endif
