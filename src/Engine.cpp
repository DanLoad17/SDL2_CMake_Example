#include "Engine.h"
#include <iostream>

Engine::Engine(const std::string& title, int width, int height)
    : title(title), width(width), height(height),
      window(nullptr), renderer(nullptr), isRunning(false),
      rectX(100), rectY(100), rectSpeed(5) // initialize rectangle position and speed
{}

bool Engine::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow(title.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    isRunning = true;
    return true;
}

void Engine::handleInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_w:
                    rectY -= rectSpeed;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    rectY += rectSpeed;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    rectX -= rectSpeed;
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    rectX += rectSpeed;
                    break;
            }
        }
    }
}

void Engine::run() {
    while (isRunning) {
        handleInput();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render();

        SDL_RenderPresent(renderer);

        // Delay to control frame rate (~60 FPS)
        SDL_Delay(16);
    }
}

void Engine::render() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_Rect rect = { rectX, rectY, 200, 150 };
    SDL_RenderFillRect(renderer, &rect);
}

void Engine::cleanup() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
