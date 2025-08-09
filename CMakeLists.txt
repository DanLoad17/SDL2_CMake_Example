cmake_minimum_required(VERSION 3.10)
project(SDL2_CMake_Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 CONFIG REQUIRED)

add_executable(SDL2_CMake_Example WIN32 main.cpp)

target_link_libraries(SDL2_CMake_Example PRIVATE SDL2::SDL2 SDL2::SDL2main)

target_include_directories(SDL2_CMake_Example PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SDL2_CMake_Example PRIVATE ${SDL2_LIBRARIES})