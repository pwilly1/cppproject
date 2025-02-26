#include "World.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "tinyxml2.h"

using namespace tinyxml2;

World::World(SDL_Renderer* renderer) {
    tilesetTexture = IMG_LoadTexture(renderer, "../../../resources/Cave Tiles/Cave Tiles.png");
    if (!tilesetTexture) {
        std::cout << "Failed to load tileset: " << SDL_GetError() << std::endl;
    }
}

void World::loadFromTMX(const std::string& filename) {

    //loading the XML doc from tmx file
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cout << "Failed to load TMX file: " << filename << std::endl;
        return;
    }

    //getting mapelement from tmx file map
    XMLElement* mapElement = doc.FirstChildElement("map");
    if (!mapElement) {
        std::cout << "Error: No map element found in TMX!" << std::endl;
        return;
    }

    //retreiving information from mapElement

    mapWidth = mapElement->IntAttribute("width");
    mapHeight = mapElement->IntAttribute("height");
    tileSize = mapElement->IntAttribute("tilewidth");


    //get tileset info from map element
    // Parse tilesets
    XMLElement* tilesetElement = mapElement->FirstChildElement("tileset");
    parseTileset(tilesetElement);

    // Parse layers
    XMLElement* layerElement = mapElement->FirstChildElement("layer");
    parseLayer(layerElement);

    // Store layer offsets
    layerOffsetX = layerElement->FloatAttribute("offsetx", 0);
    layerOffsetY = layerElement->FloatAttribute("offsety", 0);
}

void World::parseTileset(XMLElement* tilesetElement) {
    if (!tilesetElement) return;
    int firstGid = tilesetElement->IntAttribute("firstgid");
    std::cout << "Tileset first GID: " << firstGid << std::endl;
}

void World::parseLayer(XMLElement* layerElement) {
    if (!layerElement) return;

    XMLElement* dataElement = layerElement->FirstChildElement("data");
    if (!dataElement) return;

    tiles.clear(); // Clear previous map data

    int minChunkX = INT_MAX, minChunkY = INT_MAX;
    int maxChunkX = INT_MIN, maxChunkY = INT_MIN;

    // First pass: Find min/max chunk coordinates
    for (XMLElement* chunkElement = dataElement->FirstChildElement("chunk");
        chunkElement;
        chunkElement = chunkElement->NextSiblingElement("chunk")) {
        int chunkX = chunkElement->IntAttribute("x");
        int chunkY = chunkElement->IntAttribute("y");
        int chunkWidth = chunkElement->IntAttribute("width");
        int chunkHeight = chunkElement->IntAttribute("height");

        minChunkX = std::min(minChunkX, chunkX);
        minChunkY = std::min(minChunkY, chunkY);
        maxChunkX = std::max(maxChunkX, chunkX + chunkWidth);
        maxChunkY = std::max(maxChunkY, chunkY + chunkHeight);
    }

    // Calculate correct map size
    mapWidth = maxChunkX - minChunkX;
    mapHeight = maxChunkY - minChunkY;

    //resize 2d vector, first arg = row, second arg = columns
    tiles.resize(mapHeight, std::vector<int>(mapWidth, 0)); // Resize map dynamically

    std::cout << "Determined Map Size: " << mapWidth << "x" << mapHeight << std::endl;

    // Second pass: Process chunks with adjusted offsets
    for (XMLElement* chunkElement = dataElement->FirstChildElement("chunk");
        chunkElement;
        chunkElement = chunkElement->NextSiblingElement("chunk")) {

        int chunkX = chunkElement->IntAttribute("x") - minChunkX; // Adjusted to fit within the new map
        int chunkY = chunkElement->IntAttribute("y") - minChunkY;
        int chunkWidth = chunkElement->IntAttribute("width");
        int chunkHeight = chunkElement->IntAttribute("height");

        std::cout << "Processing chunk at (" << chunkX << ", " << chunkY << ") size: "
            << chunkWidth << "x" << chunkHeight << std::endl;

        std::string csvData = chunkElement->GetText();
        std::istringstream dataStream(csvData);
        std::string tile;

        for (int y = 0; y < chunkHeight; y++) {
            for (int x = 0; x < chunkWidth; x++) {
                //get values from dataStream and put in tile
                if (!std::getline(dataStream, tile, ',')) {
                    std::cout << "Unexpected end of CSV data while reading chunk" << std::endl;
                    break;
                }
                //convert string to int
                int tileValue = std::stoi(tile);

                int globalX = chunkX + x;
                int globalY = chunkY + y;

                if (globalX >= 0 && globalX < mapWidth && globalY >= 0 && globalY < mapHeight) {
                    tiles[globalY][globalX] = tileValue;
                }
                else {
                    std::cout << "Skipping out-of-bounds tile (" << globalX << ", " << globalY << ")" << std::endl;
                }
            }
        }
    }

    std::cout << "Successfully Loaded Infinite TMX Map!" << std::endl;
}



void World::render(SDL_Renderer* renderer, int cameraX, int cameraY, int screenWidth, int screenHeight) {
    int startX = cameraX / tileSize;
    int startY = cameraY / tileSize;
    int endX = startX + (screenWidth / tileSize) + 1;
    int endY = startY + (screenHeight / tileSize) + 1;

    SDL_FRect srcTile;

    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            if (x >= 0 && y >= 0 && x < tiles[0].size() && y < tiles.size()) {
                int tileID = tiles[y][x]; // Y comes first in 2D arrays

                if (tileID == 0) continue;  // Skip empty tiles

                float tileX = ((tileID - 1) % 8) * tileSize;  // Adjusting GID
                float tileY = ((tileID - 1) / 8) * tileSize;

                srcTile = { tileX, tileY, tileSize, tileSize };
                SDL_FRect destTile = { (x * tileSize - cameraX) + layerOffsetX,
                                       (y * tileSize - cameraY) + layerOffsetY,
                                       tileSize, tileSize };

                SDL_RenderTexture(renderer, tilesetTexture, &srcTile, &destTile);
            }
        }
    }
}




