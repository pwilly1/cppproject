cmake_minimum_required(VERSION 3.10)
project(cppproject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL3
set(SDL3_DIR "C:/Users/prwil/OneDrive/Documents/SDL3/SDL3-3.2.4")
include_directories(${SDL3_DIR}/include)
link_directories(${SDL3_DIR}/lib/x64)

# Add the source files
add_executable(cppproject main.cpp "Game.cpp" "Game.h" "GameObject.cpp" "GameObject.h")

# Find and Link SDL3 Properly
set(CMAKE_PREFIX_PATH "C:/Users/prwil/OneDrive/Documents/SDL3/SDL3-3.2.4/cmake")
find_package(SDL3 REQUIRED)
target_link_libraries(cppproject SDL3::SDL3)

set(CMAKE_PREFIX_PATH "C:/Users/prwil/OneDrive/Documents/SDL3/image/SDL3_image-3.2.0/cmake")
find_package(SDL3_image REQUIRED)
target_link_libraries(cppproject SDL3_image::SDL3_image)

