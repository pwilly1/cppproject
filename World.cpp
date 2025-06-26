#include "World.h"
#include "GameObject.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "tinyxml2.h"
#include <filesystem>  // C++17
    
using namespace tinyxml2;

World::World(SDL_Renderer* renderer) {  
    std::cout << "Attempting to load tileset texture from: ../../../resources/Cave Tiles/Cave Tiles.png" << std::endl;
    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;

    tilesetTexture = IMG_LoadTexture(renderer, "C:/Users/prwil/source/repos/pwilly1/cppproject/resources/Cave Tiles/Cave Tiles.png");
    if (!tilesetTexture) {
        std::cout << "Failed to load tileset(world.cpp): " << SDL_GetError() << std::endl;
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
    std::cout << "map element" << mapElement << std::endl;

    if (!mapElement) {
        std::cout << "Error: No map element found in TMX!" << std::endl;
        return;
    }

    //retreiving information from mapElement



	//mapwidth and mapheight from tmx file are useless because I am using infinite map mode so I will calculate correct map size based on chunks in the parsing
    mapWidth = mapElement->IntAttribute("width");
    std::cout << "Map Width: " << mapWidth << std::endl;

    mapHeight = mapElement->IntAttribute("height");
    tileSize = mapElement->IntAttribute("tilewidth");


    //get tileset info from map element
    // Parse tilesets
    XMLElement* tilesetElement = mapElement->FirstChildElement("tileset");
    parseTileset(tilesetElement);

    // Parse layers
    XMLElement* groupElement = mapElement->FirstChildElement("group");
    if (!groupElement) {
        std::cout << "No group element found." << std::endl;
        return;
    }

    XMLElement* layerElement = groupElement->FirstChildElement("layer");
    if (!layerElement) {
        std::cout << "No layer element found." << std::endl;
        return;
    }
    parseLayer(layerElement);
   
    XMLElement* collisionLayer = groupElement->FirstChildElement("objectgroup");
    parseCollisionLayer(collisionLayer, layerElement);


  
}





void World::parseTileset(XMLElement* tilesetElement) {
    if (!tilesetElement) return;
    int firstGid = tilesetElement->IntAttribute("firstgid");
    std::cout << "Tileset first GID: " << firstGid << std::endl;
}






void World::parseLayer(XMLElement* layerElement) {

	std::cout << "starting parseLayer" << std::endl;
    if (!layerElement) return;

    XMLElement* dataElement = layerElement->FirstChildElement("data");
    if (!dataElement) return;

    tiles.clear(); // Clear previous map data

    minChunkX = INT_MAX;
    minChunkY = INT_MAX;
    int maxChunkX = INT_MIN;
    int maxChunkY = INT_MIN;



    // First pass: Find min/max chunk coordinates
    for (XMLElement* chunkElement = dataElement->FirstChildElement("chunk");
        chunkElement;
        chunkElement = chunkElement->NextSiblingElement("chunk")) {
        std::cout << "starting first pass" << std::endl;


        //Chunk x and y are the top left of the chunk
        int chunkX = chunkElement->IntAttribute("x");
		std::cout << "chunkX: " << chunkX << std::endl;
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
        std::cout << "minCHUNKKKKKKKKKKKK" << this->minChunkX << std::endl;

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


        //I am dividing by tilesize because the tmx file has the coordinates as normal number coordinates
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

        std::cout << "minCHUNKKKKKKKKKKKK" << this->minChunkX << std::endl;

        int x = static_cast<int>(std::round(xRaw / tileSize)) - this->minChunkX; // - minX;  // Normalize X
        int y = static_cast<int>(std::round(yRaw / tileSize)) - this->minChunkY; //- minY;  // Normalize Y
        int width = static_cast<int>(std::round(widthRaw / tileSize));
        int height = static_cast<int>(std::round(heightRaw / tileSize));

        if (width == 0) width = 1;
        if (height == 0) height = 1;

        std::cout << "Processing collision object at (" << x << ", " << y << ") size: "
            << width * tileSize << "x" << height * tileSize << std::endl;
     

        // Step 4: Mark the collision tiles in the normalized map
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                int tileX = (x + i); 
                int tileY = (y + j); 
                //std::cout << "y j: " << y + j << "tiley: " << tileY << "off: " << layerOffsetY << std::endl;
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









void World::breakTile(int x, int y) {
    // Convert coordinates to tile indices
    int tileX = x / tileSize;
    int tileY = y / tileSize;
    // Check if the tile is within bounds
    if (tileX >= 0 && tileX < mapWidth && tileY >= 0 && tileY < mapHeight) {
        // Set the tile to 0 (empty)
        tiles[tileY][tileX] = 0;
        collisionMap[tileY][tileX] = 0;
        std::cout << "Tile at (" << tileX << ", " << tileY << ") broken." << std::endl;
    } else {
        std::cout << "Tile coordinates out of bounds: (" << tileX << ", " << tileY << ")" << std::endl;
    }
}






//AABB Wall Collision Detection

void World::checkWallCollisons(GameObject& p, float cameraX, float cameraY) {

    for (int i = 0; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            if (collisionMap[i][j] == true) {
                // calculating distance from player and collision tile using their centers
                //I am using tileSize instead of tile width and tile height because my tiles are sqaures but If I wanted to not use sqaure I would have to use tile height and width instead of tileSize
                int distX = floor(p.getX() + p.getPlayerWidth() / 2) - (j * tileSize + tileSize / 2);
                int distY = floor(p.getY() + p.getPlayerHeight() / 2) - (i * tileSize + tileSize / 2);
                int combinedHalfW = floor(p.getPlayerWidth() / 2) + (tileSize / 2);
                int combinedHalfH = floor(p.getPlayerHeight() / 2) + (tileSize / 2);

                //check for x overlap
                if (abs(distX) < combinedHalfW) {

                    //check for y overlap
                    if (abs(distY) < combinedHalfH) {
                        int overlapX = combinedHalfW - abs(distX);
                        int overlapY = combinedHalfH - abs(distY);
                        //we need to look for smallest overlap
                        if (overlapX >= overlapY) {
                            //correct Y position
                            //intersecting top side because if cell is above player that means player height - cell position is positive
                            if (distY > 0) {
                                p.setY(p.getY() + overlapY); //move down
                            }
                            else {
								// std::cout << "is on ground" << std::endl;
                                isOnGround();
                                p.setY(p.getY() - overlapY); //move up
                            }

                            // NEW FIX:
                            p.setdy(0);  // stop vertical velocity after vertical collision
                        }//end Y corrections
                        else {
                            //correct X position
                            //intersecting left side because if cell is to the left of player that means player width - cell position is positive
                            if (distX > 0) {
                                p.setX(p.getX() + overlapX); //move right
                            }
                            else {
                                p.setX(p.getX() - overlapX); //move left
                            }
                        }
                    }//end y overlap
                }//end x overlap


            }//endcollisionMap
        }//end columns
    }//end rows
}

bool World::isOnGround() {
    return true;
}




void World::render(SDL_Renderer* renderer, int cameraX, int cameraY, int screenWidth, int screenHeight) {
    if (tiles.empty() || tiles[0].empty()) {
        std::cout << "Tiles vector is not properly initialized." << std::endl;
    }

    int startX = cameraX / tileSize;
    int startY = cameraY / tileSize;
    int endX = startX + (screenWidth / tileSize); // calculate how big the render array will be
    int endY = startY + (screenHeight / tileSize);
    //tileLocationsX.resize(endX);
    //tileLocationsY.resize(endY);

    SDL_FRect srcTile;

    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            if (y >= 0 && y < tiles.size() && x >= 0 && x < tiles[0].size()){

            // if (x >= 0 && y >= 0 && x < tiles[0].size() && y < tiles.size()) {
                int tileID = tiles[y][x]; // Y comes first in 2D arrays

                if (tileID == 0) continue;  // Skip empty tiles

                //find the postion of tile inside the tileset texture
                //get column index
                float tileX = (tileID) % 8 * tileSize;  // Adjusting GID
                //get row index
                float tileY = floor((tileID) / 16) * tileSize;

                //portion of tileset imag eto be drawn
                srcTile = { tileX, tileY, tileSize, tileSize };

                //locations where the tiles will be drawn
                int tileLocationX = (x * tileSize) - cameraX;
                int tileLocationY = (y * tileSize) - cameraY; // (y * tileSize - cameraY) + layerOffsetY;


                //where the tile will be drawn
                SDL_FRect destTile = { static_cast<float>(tileLocationX), static_cast<float>(tileLocationY), tileSize, tileSize };


               // tileLocationsX[y] = tileLocationX;
               // tileLocationsY[y] = tileLocationY;

                SDL_RenderTexture(renderer, tilesetTexture, &srcTile, &destTile);
            }
        }
    }
    // Enable transparency for collision overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Render collision map overlay
    /*SDL_SetRenderDrawColor(renderer, 255, 0, 0, 120);  // Red with 120 alpha

    //map to show where the actual collision is
    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            if (y >= 0 && y < collisionMap.size() && x >= 0 && x < collisionMap[0].size()) {
            // if (x >= 0 && y >= 0 && x < collisionMap[0].size() && y < collisionMap.size()) {
                if (collisionMap[y][x]) {

                        //location where tiles will be drawn
                    int tileLocationX = (x * tileSize) - cameraX;
                    int tileLocationY = (y * tileSize) - cameraY;
                    SDL_FRect collisionBox = { static_cast<float>(tileLocationX), static_cast<float>(tileLocationY), tileSize, tileSize };

                    SDL_RenderFillRect(renderer, &collisionBox);
                }
            }
        }
    }
    */
    // Reset color to white after rendering collision map
}




