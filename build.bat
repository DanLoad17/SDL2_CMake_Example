@echo off
REM Change this path to your actual vcpkg installation path
set VCPKG_PATH=C:\Users\goran\vcpkg

REM Create build directory if it doesn't exist
if not exist build (
    mkdir build
)

REM Run CMake configuration with vcpkg toolchain
cmake -Bbuild -S. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_PATH%\scripts\buildsystems\vcpkg.cmake

REM Build the project
cmake --build build

pause

REM RUN THIS FILE AS .\build.bat TO MAKE THE EXE