cmake_minimum_required(VERSION 3.10)
project(SDL2_CMake_Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SRC_FILES src/*.cpp)

add_executable(SDL2_CMake_Example ${SRC_FILES})

target_link_libraries(SDL2_CMake_Example PRIVATE SDL2::SDL2 SDL2::SDL2main)
