cmake_minimum_required(VERSION 3.15) 
project(cppproject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Use vcpkg for dependencies



# Add the source files
add_executable(cppproject "src/main.cpp" "src/Game.cpp" "src/Game.h" "src/GameObject.cpp" "src/GameObject.h" "src/World.h" "src/World.cpp" "src/HUDManager.cpp" "src/HUDManager.h" "src/Inventory.cpp" "src/Inventory.h" "src/InventoryItem.h" "src/InventoryItem.cpp" "src/Player.cpp" "src/Player.h" "src/EnemyNPC.h" "src/EnemyNPC.cpp" "src/BasicEnemy.cpp" "src/BasicEnemy.h")

# SDL (use CONFIG mode)
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

# TinyXML2 (CONFIG mode, imported target)
find_package(TinyXML2 REQUIRED CONFIG)

target_link_libraries(cppproject
  PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    tinyxml2::tinyxml2
)

