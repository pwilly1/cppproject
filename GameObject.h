#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

class GameObject {

public:

    GameObject(int x, int y, int w, int h, SDL_Renderer* renderer, const std::string& imagePath);
    ~GameObject();
    void update(float deltaTime);  //  Updates movement
    void render(SDL_Renderer* renderer); //  Draws the object
    void setVelocity(int dx, int dy); // Sets movement speed
    int getSpeed() const { return speed; }
    int getdx() { return dx; }
    int getdy() { return dy; }


private:
    SDL_FRect srcRect;  // Source rectangle for cropping the sprite sheet
    SDL_FRect destRect;
    int x, y; //  Position
    int width, height; //  Size
    int dx, dy; //  Velocity
    SDL_Texture* texture;  // Store the image texture
    int speed;
};

#endif
