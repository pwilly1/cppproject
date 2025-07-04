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
    ~GameObject();
    void update(float deltaTime, World& world);  //  Updates movement
    void render(SDL_Renderer* renderer, float cameraX, float cameraY); //  Draws the object
    void setVelocity(int dx, int dy); // Sets movement speed
    int getSpeed() const { return speed; }
    int getdx() { return dx; }
    int getdy() { return dy; }
    bool getIsJumping();
    void jump(World& world);
    //bool isOnGround();
    void SetGroundLevel();
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    int getX() { return x; }
    int getY() { return y; }
    int getPlayerHeight() { return height; }
    int getPlayerWidth() { return width; }
    void setdy(float dy) { this->dy = dy; }
    bool getBreakMode() { return breakMode; }
    void setBreakMode(bool set) { breakMode = set; }
    void toggleBreakMode();
    bool getPlaceMode() const { return placeMode; }
    void setPlaceMode(bool set) { placeMode = set; }

    


private:
    Inventory* inventory;
    bool placeMode = false;
	bool breakMode = false;
	int stoneCollected = 0;
    bool isJumping = false;
    float jumpPower = 300;
    const float gravity = 400;
    float groundLevel = 450;
    SDL_FRect srcRect;  // Source rectangle for cropping the sprite sheet
    SDL_FRect destRect;
    float x, y; //  Position
    int width, height; //  Size
    int dx, dy; //  Velocity
    SDL_Texture* texture;  // Store the image texture
    int speed;
};

#endif
