#pragma once
#include <SDL.h>
#include <string>

class Engine {
public:
    Engine(const std::string& title, int width, int height);
    bool init();
    void run();
    void cleanup();

private:
    std::string title;
    int width;
    int height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
};
