#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <cmath>


class GameObject {

public:

    GameObject(int x, int y, SDL_Renderer* renderer, const std::string& imagePath);
    ~GameObject();
    void update(float deltaTime);  //  Updates movement
    void render(SDL_Renderer* renderer); //  Draws the object
    void setVelocity(int dx, int dy); // Sets movement speed
    int getSpeed() const { return speed; }
    int getdx() { return dx; }
    int getdy() { return dy; }
    bool getIsJumping();
    void jump();
    bool isOnGround();
    void SetGroundLevel();
    int getX() { return x; }
    int getY() { return y; }
    int getPlayerHeight() { return height; }
    int getPlayerWidth() { return width; }
    


private:
    bool isJumping = false;
    float jumpPower = 300;
    const float gravity = 400;
    float groundLevel = 400.0;
    SDL_FRect srcRect;  // Source rectangle for cropping the sprite sheet
    SDL_FRect destRect;
    float x, y; //  Position
    int width, height; //  Size
    int dx, dy; //  Velocity
    SDL_Texture* texture;  // Store the image texture
    int speed;
};

#endif
