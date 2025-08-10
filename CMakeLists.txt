cmake_minimum_required(VERSION 3.10)
project(SDL2_CMake_Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2 packages
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SRC_FILES src/*.cpp)

add_executable(SDL2_CMake_Example ${SRC_FILES})

# Link SDL2, SDL2_ttf, SDL2_image
target_link_libraries(SDL2_CMake_Example
    PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
        SDL2_image::SDL2_image
)
