#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
class World;
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <cmath>
class Inventory;

class GameObject {

public:

    GameObject(int x, int y, SDL_Renderer* renderer, const std::string& imagePath);
    virtual ~GameObject();

    virtual void update(float deltaTime);
    virtual void render(SDL_Renderer* renderer, float cameraX, float cameraY);
    void jump();

    void setVelocity(int dx, int dy);
    void setX(float x) { this->x = x; }
    void setY(float y) { this->y = y; }
    void setdx(float dx) { this->dx = dx;  }
    void setdy(float dy) { this->dy = dy; }
    void setIsOnGround(bool set) { isOnGround = set; }
    void setHorozontalCollision(bool set) { horozontalCollision = set;  }

    bool getIsJumping();
    bool getHorozontalCollision() { return horozontalCollision; }
    float getX() const { return x; }
    float getY() const { return y; }
    int getdx() const { return dx; }
    int getdy() const { return dy; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getSpeed() const { return speed; }
    SDL_FRect getDestRect() { return destRect; }

    SDL_FRect srcRect, destRect;

protected:
    SDL_Texture* texture = nullptr;

    float x, y;
    int width, height = 0;
    int dx = 0, dy = 0;
    int speed = 99;

    float gravity = 400.0f;

    void applyPhysics(float deltaTime);

private:
    bool horozontalCollision = false;
    bool isOnGround = false;
    bool isJumping = false;
    float jumpPower = 300;
};

#endif
