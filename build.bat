@echo off
REM Adjust VCPKG_PATH if your vcpkg is installed somewhere else
set VCPKG_PATH=%USERPROFILE%\vcpkg

REM Create build directory if it doesn't exist
if not exist build (
    mkdir build
)

REM Configure CMake with vcpkg toolchain
cmake -Bbuild -S. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_PATH%\scripts\buildsystems\vcpkg.cmake

REM Build the project
cmake --build build --clean-first

pause
