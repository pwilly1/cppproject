#ifndef WORLD_H
#define WORLD_H

#include "GameObject.h"
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "tinyxml2.h"

class World {
public:
    World(SDL_Renderer* renderer);
    void loadFromTMX(const std::string& filename);
    void render(SDL_Renderer* renderer, int cameraX, int cameraY, int screenWidth, int screenHeight);
    std::vector<int> getTileLocationsX() { return tileLocationsX; }
    std::vector<int> getTileLocationsY() { return tileLocationsY; }
    int getMinChunkY() { return minChunkY;}
    int getMinChunkX() { return minChunkX; }

    bool isCollidable(int x, int y); // New function to check collision

    void checkWallCollisons(GameObject& p);
private:
    std::vector<int> tileLocationsX; //tile locations for rendering
    std::vector<int> tileLocationsY;
    SDL_Texture* tilesetTexture;
    float tileSize;
    int mapWidth, mapHeight;
    float layerOffsetX, layerOffsetY;
    int minChunkX, minChunkY;

    std::vector<std::vector<int>> tiles;
    std::vector<std::vector<bool>> collisionMap; // Collision map

    //void World::updateCollisionMap(int cameraX, int cameraY, int screenWidth, int screenHeight);

    void parseTileset(tinyxml2::XMLElement* tilesetElement);
    void parseLayer(tinyxml2::XMLElement* layerElement);
    void World::parseCollisionLayer(tinyxml2::XMLElement* objectGroupElement, tinyxml2::XMLElement* layerElement);

};

#endif // WORLD_H
