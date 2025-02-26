#ifndef WORLD_H
#define WORLD_H

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

private:
    SDL_Texture* tilesetTexture;
    float tileSize;
    int mapWidth, mapHeight;
    float layerOffsetX, layerOffsetY;

    std::vector<std::vector<int>> tiles;

    void parseTileset(tinyxml2::XMLElement* tilesetElement);
    void parseLayer(tinyxml2::XMLElement* layerElement);
};

#endif // WORLD_H
