# SDL3 Platformer/Game Engine Project (NOT PORTABLE YET)

This is my SDL3-based platformer game built with C++, SDL3, and TinyXML2.

![Demo](resources/game-ezgif.com-video-to-gif-converter.gif)

-----------------------------------------------------------------------------

## Requirements

- CMake
- C++ Compiler (Visual Studio / g++ / clang)
- vcpkg

------------------------------------------------------------------------------

## Dependencies

Handled automatically by vcpkg:

- SDL3
- SDL3_image
- tinyxml2

------------------------------------------------------------------------------

## Build Instructions

*If you don't want to build it yourself there is a folder called builds with cppproject.exe inside. You can run that exe to play the game.*

### Clone the repository(NOT PORTABLE YET):

```bash
git clone https://github.com/pwilly1/cppproject.git
cd cppproject
vcpkg install
cmake -B build -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
