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
    XMLElement* collisionLayer = mapElement->FirstChildElement("objectgroup");
    parseCollisionLayer(collisionLayer, layerElement);


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

        //Chunk x and y are the top left of the chunk
        int chunkX = chunkElement->IntAttribute("x");
        int chunkY = chunkElement->IntAttribute("y");
        int chunkWidth = chunkElement->IntAttribute("width");
        int chunkHeight = chunkElement->IntAttribute("height");

        //calculate min and max of whole map
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

        //subtract min x and y so all chunks start relative to (0,0)
        int chunkX = chunkElement->IntAttribute("x") - minChunkX; 
        int chunkY = chunkElement->IntAttribute("y") - minChunkY;
        int chunkWidth = chunkElement->IntAttribute("width");
        int chunkHeight = chunkElement->IntAttribute("height");

        std::cout << "Processing chunk at (" << chunkX << ", " << chunkY << ") size: "
            << chunkWidth << "x" << chunkHeight << std::endl;

        std::string csvData = chunkElement->GetText();
        //converts chunk data into stream so I can read individual tile values
        std::istringstream dataStream(csvData);
        std::string tile;

        for (int y = 0; y < chunkHeight; y++) {
            for (int x = 0; x < chunkWidth; x++) {
                //get values from dataStream and put in tile one at a time comma as delimiter
                if (!std::getline(dataStream, tile, ',')) {
                    std::cout << "Unexpected end of CSV data while reading chunk" << std::endl;
                    break;
                }
                //convert string to int
                int tileValue = std::stoi(tile);

                //where to put the tile
                int globalX = chunkX + x;
                int globalY = chunkY + y;

                //put tile values in tiles array

                if (globalX >= 0 && globalX < mapWidth && globalY >= 0 && globalY < mapHeight) {
                    tiles[globalY][globalX] = tileValue;
                }
                else {
                    std::cout << "Skipping out-of-bounds tile (" << globalX << ", " << globalY << ")" << std::endl;
                }
            }
        }
    }
    std::cout << "Final Map Representation (" << mapWidth << "x" << mapHeight << "):\n";
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (tiles[y][x] == 0) {
                std::cout << "."; // Empty tile
            }
            else {
                std::cout << tiles[y][x] % 10; // Print last digit of tile ID for clarity
            }
        }
        std::cout << std::endl;
    }


    std::cout << "Successfully Loaded Infinite TMX Map!" << std::endl;
}



void World::parseCollisionLayer(tinyxml2::XMLElement* objectGroupElement, tinyxml2::XMLElement* layerElement) {
    if (!objectGroupElement) return;

    // Step 1: Find min/max coordinates for normalization
    int minX = INT_MAX, minY = INT_MAX;
    int maxX = INT_MIN, maxY = INT_MIN;

    for (tinyxml2::XMLElement* objectElement = objectGroupElement->FirstChildElement("object");
        objectElement;
        objectElement = objectElement->NextSiblingElement("object")) {

        float xRaw = objectElement->FloatAttribute("x");
        float yRaw = objectElement->FloatAttribute("y");
        float widthRaw = objectElement->FloatAttribute("width", tileSize);
        float heightRaw = objectElement->FloatAttribute("height", tileSize);

        int x = static_cast<int>(std::round(xRaw / tileSize));
        int y = static_cast<int>(std::round(yRaw / tileSize));
        int width = static_cast<int>(std::round(widthRaw / tileSize));
        int height = static_cast<int>(std::round(heightRaw / tileSize));

        if (width == 0) width = 1;
        if (height == 0) height = 1;

        minX = std::min(minX, x);
        minY = std::min(minY, y);
        maxX = std::max(maxX, x + width);
        maxY = std::max(maxY, y + height);
    }

    // Step 2: Normalize the collision map dimensions
    
    collisionMap.clear();
    collisionMap.resize(mapHeight, std::vector<bool>(mapWidth, false));

    std::cout << "Determined Collision Map Size: " << mapWidth << "x" << mapHeight << std::endl;

    // Step 3: Process objects with adjusted offsets
    for (tinyxml2::XMLElement* objectElement = objectGroupElement->FirstChildElement("object");
        objectElement;
        objectElement = objectElement->NextSiblingElement("object")) {

        float xRaw = objectElement->FloatAttribute("x");
        float yRaw = objectElement->FloatAttribute("y");
        float widthRaw = objectElement->FloatAttribute("width", tileSize);
        float heightRaw = objectElement->FloatAttribute("height", tileSize);

        int x = static_cast<int>(std::round(xRaw / tileSize)) - minX;  // Normalize X
        int y = static_cast<int>(std::round(yRaw / tileSize)) - minY;  // Normalize Y
        int width = static_cast<int>(std::round(widthRaw / tileSize));
        int height = static_cast<int>(std::round(heightRaw / tileSize));

        if (width == 0) width = 1;
        if (height == 0) height = 1;

        std::cout << "Processing collision object at (" << x << ", " << y << ") size: "
            << width * tileSize << "x" << height * tileSize << std::endl;
        layerOffsetY = layerElement->FloatAttribute("offsety", 0);
        layerOffsetX = layerElement->FloatAttribute("offsetx", 0);

        // Step 4: Mark the collision tiles in the normalized map
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                int tileX = (x + i) - (layerOffsetX / tileSize);
                int tileY = (y + j) + (layerOffsetY/tileSize);
                std::cout << "y j: " << y + j << "tiley: " << tileY << "off: " << layerOffsetY << std::endl;
                if (tileX >= 0 && tileX < mapWidth && tileY >= 0 && tileY < mapHeight) {
                    collisionMap[tileY][tileX] = true;
                }
            }
        }
    }

    // Step 5: Print the final collision map
    std::cout << "Final Collision Map (" << mapWidth << "x" << mapHeight << "):\n";
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            std::cout << (collisionMap[y][x] ? "#" : ".");
        }
        std::cout << std::endl;
    }

    std::cout << "Successfully Loaded Collision Layer!" << std::endl;
}




// Function to check if a tile is solid
bool World::isCollidable(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) {
        return false;
    }
    return collisionMap[y][x];
}








void World::render(SDL_Renderer* renderer, int cameraX, int cameraY, int screenWidth, int screenHeight) {
    int startX = cameraX / tileSize;
    int startY = cameraY / tileSize;
    int endX = startX + (screenWidth / tileSize);
    int endY = startY + (screenHeight / tileSize);
    tileLocationsX.resize(endX);
    tileLocationsY.resize(endY);

    SDL_FRect srcTile;

    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            if (x >= 0 && y >= 0 && x < tiles[0].size() && y < tiles.size()) {
                int tileID = tiles[y][x]; // Y comes first in 2D arrays

                if (tileID == 0) continue;  // Skip empty tiles

                //find the postion of tile inside the tileset texture
                //get column index
                float tileX = ((tileID - 1) % 8) * tileSize;  // Adjusting GID
                //get row index
                float tileY = ((tileID - 1) / 8) * tileSize;

                //portion of tileset imag eto be drawn
                srcTile = { tileX, tileY, tileSize, tileSize };

                //locations where the tiles will be drawn
                int tileLocationX = (x * tileSize - cameraX) + layerOffsetX;
                int tileLocationY = (y * tileSize - cameraY) + layerOffsetY;


                //where the tile will be drawn
                SDL_FRect destTile = { tileLocationX,tileLocationY,tileSize, tileSize };


                tileLocationsX[y] = tileLocationX;
                tileLocationsY[y] = tileLocationY;

                SDL_RenderTexture(renderer, tilesetTexture, &srcTile, &destTile);
            }
        }
    }
    // Enable transparency for collision overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Render collision map overlay
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 120);  // Red with 120 alpha

    //map to show where the actual collision is from parsecollisionlayer
    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            if (x >= 0 && y >= 0 && x < collisionMap[0].size() && y < collisionMap.size()) {
                if (collisionMap[y][x]) {
                    int tileLocationX = x * tileSize;
                        int tileLocationY = y * tileSize;

                    SDL_FRect collisionBox = { tileLocationX, tileLocationY, tileSize, tileSize };
                    SDL_RenderFillRect(renderer, &collisionBox);
                }
            }
        }
    }

    // Reset color to white after rendering collision map
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}




