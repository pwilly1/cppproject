# SDL3 Platformer Project(NOT PORTABLE YET)

This is my SDL3-based platformer game built with C++, SDL3, and TinyXML2.

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

### Clone the repository(NOT PORTABLE YET):

```bash
git clone https://github.com/pwilly1/cppproject.git
cd cppproject
vcpkg install
cmake -B build -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
