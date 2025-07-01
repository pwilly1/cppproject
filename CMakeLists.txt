cmake_minimum_required(VERSION 3.10) 
project(cppproject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Use vcpkg for dependencies
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()



# Add the source files
add_executable(cppproject main.cpp "Game.cpp" "Game.h" "GameObject.cpp" "GameObject.h" "World.h" "World.cpp" "HUDManager.cpp" "HUDManager.h" "Inventory.cpp" "InventoryItem.h" "InventoryItem.cpp")

# Find and Link SDL3
find_package(SDL3 REQUIRED)
target_link_libraries(cppproject SDL3::SDL3)

# Find and Link SDL3_image
set(CMAKE_PREFIX_PATH "C:/Users/prwil/source/repos/pwilly1/cppproject/vcpkg_installed/x64-windows")
include_directories("C:/Users/prwil/source/repos/pwilly1/cppproject/vcpkg_installed/x64-windows/include")
find_package(SDL3_image REQUIRED)
target_link_libraries(cppproject SDL3_image::SDL3_image)

# Find and Link TinyXML2 from vcpkg
set(CMAKE_PREFIX_PATH "C:/Users/prwil/source/repos/pwilly1/cppproject/vcpkg_installed/x64-windows")
include_directories("C:/Users/prwil/source/repos/pwilly1/cppproject/vcpkg_installed/x64-windows/include")
find_package(TinyXML2 REQUIRED)
target_link_libraries(cppproject TinyXML2)

find_package(SDL3_ttf REQUIRED)
target_link_libraries(cppproject SDL3_ttf::SDL3_ttf)

