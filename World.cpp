#include "World.h"
#include "GameObject.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "tinyxml2.h"
#include <filesystem>  // C++17
#include "Inventory.h"
#include <cstdlib>
#include <ctime>
    
using namespace tinyxml2;

World::World(SDL_Renderer* renderer) {  
    std::cout << "Attempting to load tileset texture from: ../../../resources/Cave Tiles/Cave Tiles.png" << std::endl;
    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
    this->renderer = renderer;
    tilesetTexture = IMG_LoadTexture(renderer, "C:/Users/prwil/source/repos/pwilly1/cppproject/resources/Cave Tiles/Cave Tiles.png");
    if (!tilesetTexture) {
        std::cout << "Failed to load tileset(world.cpp): " << SDL_GetError() << std::endl;
    }
}

World::~World() {
    if (tilesetTexture) {
        SDL_DestroyTexture(tilesetTexture);
        tilesetTexture = nullptr;
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


	//mapwidth and mapheight from tmx file are useless because I am using infinite map mode so I will calculate correct map size based on chunks in the parsing
    // mapWidth = mapElement->IntAttribute("width");
    // std::cout << "Map Width: " << mapWidth << std::endl;
    // mapHeight = mapElement->IntAttribute("height");
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
    generateMap(100, 100);
    parseLayer(layerElement);

   //  XMLElement* collisionLayer = groupElement->FirstChildElement("objectgroup");
   // parseCollisionLayer(collisionLayer, layerElement);


  
}





void World::parseTileset(XMLElement* tilesetElement) {
    if (!tilesetElement) return;

    int firstGid = 0;
    XMLDocument tileSet;

    if (tileSet.LoadFile("../../../resources/dirt.tsx") != XML_SUCCESS) {
        std::cout << "Failed to load TMX file: " << "tileset" << std::endl;
        return;
    }
    XMLElement* tsxRoot = tileSet.FirstChildElement("tileset");
    if (!tsxRoot) {
        std::cout << "Invalid tileset file format: " << std::endl;
        return;
    }

    // Loop through all tiles
    for (XMLElement* tileElement = tsxRoot->FirstChildElement("tile");
        tileElement;
        tileElement = tileElement->NextSiblingElement("tile")) {

        int tileID = tileElement->IntAttribute("id");

        XMLElement* objectGroup = tileElement->FirstChildElement("objectgroup");
        if (objectGroup) {
            collisionTileIDs.insert(firstGid + tileID); // Global ID
        }
    }

    std::cout << "Parsed tile-based collisions from: "  << std::endl;
}



void World::loadBackground(const std::string& path, const std::string& background) {
   /* auto it = backgroundTextures.find(background);
    if (it != backgroundTextures.end() && it->second) {
        SDL_DestroyTexture(it->second);
        it->second = nullptr;
    }
    */
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path.c_str());
    if (!newTexture) {
        std::cout << "Failed to load background from '" << path << "': " << SDL_GetError() << std::endl;
        return;
    }
    currentBackground = "cave";
    backgroundTextures[background] = newTexture;
}


void World::updateBackgroundForPlayer(float playerX, float playerY) {
    if (playerY < 200) {
        std::cout << "surface background\n";
        loadBackground("resources/backgrounds/surface.png", "surface");
    }
    else {
        std::cout << "cave background\n";

        loadBackground("../../../resources/Background Cave/05 rocksfront.png", "cave");
    }
}



void World::fillWorld() {
    tiles.resize(mapHeight, std::vector<int>(mapWidth, static_cast<int>(TileID::STONE)));
    collisionMap.resize(mapHeight, std::vector<bool>(mapWidth, true));  // stone = solid
}

void World::generateCaves(float fillProbability, int smoothingSteps) {
    int caveAttempts = rand() % 60 + 40;  // 5 to 15 caves
    int maxCaveWidth = 40;
    int maxCaveHeight = 30;

    for (int i = 0; i < caveAttempts; ++i) {
        int caveWidth = rand() % 25 + 15;   // 15 to 30
        int caveHeight = rand() % 15 + 15;  // 10 to 20

        int startX = rand() % (mapWidth - caveWidth);
        int startY = rand() % (mapHeight - caveHeight);

        // Step 1: Random fill
        std::vector<std::vector<bool>> temp(caveWidth, std::vector<bool>(caveHeight));
        for (int y = 0; y < caveHeight; ++y) {
            for (int x = 0; x < caveWidth; ++x) {
                temp[x][y] = (rand() / (float)RAND_MAX) < fillProbability;
            }
        }

        // Step 2: Cellular automata smoothing
        for (int s = 0; s < 4; ++s) {
            std::vector<std::vector<bool>> next = temp;

            for (int y = 1; y < caveHeight - 1; ++y) {
                for (int x = 1; x < caveWidth - 1; ++x) {
                    int walls = 0;
                    for (int dy = -1; dy <= 1; ++dy)
                        for (int dx = -1; dx <= 1; ++dx)
                            if (temp[x + dx][y + dy]) walls++;

                    next[x][y] = walls >= 5;
                }
            }

            temp = next;
        }

        // Step 3: Apply to map
        for (int y = 0; y < caveHeight; ++y) {
            for (int x = 0; x < caveWidth; ++x) {
                int mapX = startX + x;
                int mapY = startY + y;

                if (mapX >= 0 && mapX < mapWidth && mapY >= 0 && mapY < mapHeight) {
                    if (temp[x][y]) {
                        tiles[mapY][mapX] = static_cast<int>(TileID::STONE);
                        collisionMap[mapY][mapX] = true;
                    }
                    else {
                        tiles[mapY][mapX] = static_cast<int>(TileID::AIR);
                        collisionMap[mapY][mapX] = false;
                    }
                }
            }
        }
    }
}


void World::generateOreVeins(int numVeins, int veinSize) {
    for (int v = 0; v < numVeins; ++v) {
        int x = rand() % mapWidth;
        int y = rand() % mapHeight;

        for (int i = 0; i < veinSize; ++i) {
            int dx = x + (rand() % 3 - 1);
            int dy = y + (rand() % 3 - 1);

            if (dx >= 0 && dx < mapWidth && dy >= 0 && dy < mapHeight) {
                if (tiles[dy][dx] == static_cast<int>(TileID::STONE)) {
                    tiles[dy][dx] = static_cast<int>(TileID::ORE);
                }
            }
        }
    }
}





void World::generateMap(int width, int height) {
 
    fillWorld();
    generateCaves();
    generateOreVeins();

}




//will use later


void World::parseLayer(XMLElement* layerElement) {

	std::cout << "starting parseLayer" << std::endl;
    if (!layerElement) return;

    XMLElement* dataElement = layerElement->FirstChildElement("data");
    if (!dataElement) return;

    customMap.clear(); // Clear previous map data

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
    int customMapWidth = maxChunkX - minChunkX;
    int customMapHeight = maxChunkY - minChunkY;

    //resize 2d vector, first arg = row, second arg = columns
    customMap.resize(customMapHeight, std::vector<bool>(customMapWidth)); // Resize map dynamically

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
                    customMap[globalY][globalX] = collisionTileIDs.count(tileValue);
                    //std::cout << tileValue << std::endl;
                }
                else {
                    std::cout << "Skipping out-of-bounds tile (" << globalX << ", " << globalY << ")" << std::endl;
                }
            }
        }
    }

    for (int t = 0; t < customMapHeight; ++t) {
        for (int g = 0; g < customMapWidth; ++g) {
            int mapX = 50 + g;
            int mapY = 30 + t;

            if (mapX >= 0 && mapX < mapWidth && mapY >= 0 && mapY < mapHeight) {
                if (customMap[t][g]) {
                    tiles[mapY][mapX] = static_cast<int>(TileID::STONE);
                    collisionMap[mapY][mapX] = true;
                }
                else {
                    tiles[mapY][mapX] = static_cast<int>(TileID::AIR);
                    collisionMap[mapY][mapX] = false;
                }
            }
        }
    }
  
}






void World::breakTile(int x, int y, Inventory* inventory) {
    // Convert coordinates to tile indices
    int tileX = x / tileSize;
    int tileY = y / tileSize;
    // Check if the tile is within bounds
    if (tileX >= 0 && tileX < mapWidth && tileY >= 0 && tileY < mapHeight) {
        // Set the tile to 0 (empty)
        if (tiles[tileY][tileX] == 21) {
                    inventory->addItem("stone", 1, "../../../resources/stone.png");
                    std::cout << "Tile at (" << tileX << ", " << tileY << ") broken." << std::endl;
                }
        tiles[tileY][tileX] = 0;
        collisionMap[tileY][tileX] = 0;
        
    } 
    else 
    {
        std::cout << "Tile coordinates out of bounds: (" << tileX << ", " << tileY << ")" << std::endl;
    }
}





void World::placeTile(int x, int y, Inventory* inventory) {
    int tileX = x / tileSize;
    int tileY = y / tileSize;

    if (tileX >= 0 && tileX < mapWidth && tileY >= 0 && tileY < mapHeight) {
        if (tiles[tileY][tileX] == 0) {
            tiles[tileY][tileX] = 22;
            collisionMap[tileY][tileX] = 1;
            inventory->removeItem("stone", 1);

        }
    }


}







//AABB Wall Collision Detection

void World::checkWallCollisons(GameObject& p, float cameraX, float cameraY) {
    //std::cout << "check wall collisions called";
    //setIsOnGround(false);

    //only check tiles around gameObject
    int startRow = std::max(0, static_cast<int>((p.getY() - 100) / tileSize));
    int endRow = std::min(mapHeight, static_cast<int>((p.getY() + p.getHeight() + 100) / tileSize));

    int startCol = std::max(0, static_cast<int>((p.getX() - 100) / tileSize));
    int endCol = std::min(mapWidth, static_cast<int>((p.getX() + p.getWidth() + 100) / tileSize));

    for (int i = startRow; i < endRow; i++) {
        for (int j = startCol; j < endCol; j++) {

            if (collisionMap[i][j] == true) {
                // calculating distance from player and collision tile using their centers
                //I am using tileSize instead of tile width and tile height because my tiles are sqaures but If I wanted to not use sqaure I would have to use tile height and width instead of tileSize
               // std::cout << "Checking object at: " << &p << ", x: " << p.getX() << ", y: " << p.getY() << std::endl;

                int distX = floor(p.getX() + p.getWidth() / 2) - (j * tileSize + tileSize / 2);
                int distY = floor(p.getY() + p.getHeight() / 2) - (i * tileSize + tileSize / 2);
                int combinedHalfW = floor(p.getWidth() / 2) + (tileSize / 2);
                int combinedHalfH = floor(p.getHeight() / 2) + (tileSize / 2);

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
                                
                                p.setY(p.getY() - overlapY); //move up
                                if (distY <= 0 && overlapY >= 0){
                                    p.setIsOnGround(true);
                                }
                            }
                            // NEW FIX:
                            p.setdy(0);  // stop vertical velocity after vertical collision
                        }//end Y corrections
                        else {
                            p.setHorozontalCollision(true);

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
    return ground;
}

void World::setCurrentBackground(const std::string& background) {
    if (backgroundTextures.count(background)) {
        currentBackground = background;
    }
    else {
        std::cout << "Background '" << background << "' not loaded yet!" << std::endl;
    }
}



void World::render(SDL_Renderer* renderer, int cameraX, int cameraY, int screenWidth, int screenHeight) {

    if (!currentBackground.empty() && backgroundTextures[currentBackground]) {
        SDL_Texture* bg = backgroundTextures[currentBackground];

        SDL_FRect dst = { 0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight) };
        SDL_RenderTexture(renderer, bg, nullptr, &dst);
    }


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
                int tileIDD = tiles[y][x]; // Y comes first in 2D arrays

                if (tileIDD == 0) continue;  // Skip empty tiles

                //find the postion of tile inside the tileset texture
                //get column index  //20    /    2
                //float tileX = ((tileIDD + 1) / (ceil(tileIDD / tileSize))) * 8;  // Adjusting GID
                float tileX = (tileIDD % 16) * 16;  // Adjusting GID

                //get row index
                float tileY = floor(tileIDD + 1 - (tileIDD % 16));

                //portion of tileset imag eto be drawn
                srcTile = { tileX, tileY, tileSize, tileSize };

                //locations where the tiles will be drawn
                int tileLocationX = (x * tileSize) - cameraX;
                int tileLocationY = (y * tileSize) - cameraY; // (y * tileSize - cameraY) + layerOffsetY;


                //where the tile will be drawn
                SDL_FRect destTile = { static_cast<float>(tileLocationX) * zoom, static_cast<float>(tileLocationY) * zoom, tileSize * zoom, tileSize * zoom };


               // tileLocationsX[y] = tileLocationX;
               // tileLocationsY[y] = tileLocationY;

                SDL_RenderTexture(renderer, tilesetTexture, &srcTile, &destTile);
            }
        }
    }
    // Enable transparency for collision overlay
   // SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

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




