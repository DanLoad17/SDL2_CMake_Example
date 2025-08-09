#pragma once
#include <SDL.h>
#include <string>

class Engine {
public:
    Engine(const std::string& title, int width, int height);
    bool init();
    void run();
    void cleanup();
    void render();
    void handleInput();

private:
    std::string title;
    int width;
    int height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    // Position and speed for the rectangle
    int rectX;
    int rectY;
    int rectSpeed;
};
