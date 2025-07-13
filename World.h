#ifndef WORLD_H
#define WORLD_H

class GameObject; // Forward declaration to avoid circular dependency
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "tinyxml2.h"
#include <unordered_set>
class Inventory;

class World {
public:

    enum class TileID {
        AIR = 0,
        STONE = 21,
        ORE = 25,
    };


    World(SDL_Renderer* renderer);
    ~World();
    void loadFromTMX(const std::string& filename);
    void generateMap(int width, int height);
    void fillWorld();
    void generateCaves(float fillProbability = 0.45f, int smoothingSteps = 5);
    void generateOreVeins(int numVeins = 30, int veinSize = 5);



    void render(SDL_Renderer* renderer, int cameraX, int cameraY, int screenWidth, int screenHeight);
    std::vector<int> getTileLocationsX() { return tileLocationsX; }
    std::vector<int> getTileLocationsY() { return tileLocationsY; }
    int getMinChunkY() { return minChunkY;}
    int getMinChunkX() { return minChunkX; }
    void checkWallCollisons(GameObject& p, float cameraX, float cameraY);
    void breakTile(int x, int y, Inventory* inventory);
    void placeTile(int x, int y, Inventory* inventory);
    void setIsOnGround(bool set) { ground = set; }
    bool isOnGround();

private:
    std::unordered_set<int> collisionTileIDs;
    float zoom = 2;
    std::vector<int> tileLocationsX; //tile locations for rendering
    std::vector<int> tileLocationsY;
    SDL_Texture* tilesetTexture;
    float tileSize = 16;
    int mapWidth = 300, mapHeight = 300;
    float layerOffsetX, layerOffsetY = 0;
    int minChunkX, minChunkY;
    bool ground = false;
    std::vector<std::vector<bool>> customMap;
    std::vector<std::vector<int>> tiles;
    std::vector<std::vector<bool>> collisionMap; // Collision map
    void parseTileset(tinyxml2::XMLElement* tilesetElement);
    void parseLayer(tinyxml2::XMLElement* layerElement);
    //void World::parseCollisionLayer(tinyxml2::XMLElement* objectGroupElement, tinyxml2::XMLElement* layerElement);

};

#endif // WORLD_H
