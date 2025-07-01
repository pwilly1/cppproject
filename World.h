#ifndef WORLD_H
#define WORLD_H

class GameObject; // Forward declaration to avoid circular dependency
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "tinyxml2.h"
#include <unordered_set>

class World {
public:
    World(SDL_Renderer* renderer);
    ~World();
    void loadFromTMX(const std::string& filename);
    void render(SDL_Renderer* renderer, int cameraX, int cameraY, int screenWidth, int screenHeight);
    std::vector<int> getTileLocationsX() { return tileLocationsX; }
    std::vector<int> getTileLocationsY() { return tileLocationsY; }
    int getMinChunkY() { return minChunkY;}
    int getMinChunkX() { return minChunkX; }
    void checkWallCollisons(GameObject& p, float cameraX, float cameraY);
    void breakTile(int x, int y);

    bool isOnGround();

private:
    std::unordered_set<int> collisionTileIDs;
    float zoom = 2;
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
