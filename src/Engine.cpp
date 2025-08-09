#include "Engine.h"
#include <iostream>
#include <sstream>

Engine::Engine(const std::string& title, int width, int height)
    : title(title), width(width), height(height),
      window(nullptr), renderer(nullptr), isRunning(false),
      rectX(100), rectY(100), rectSpeed(5),
      font(nullptr), fpsTexture(nullptr), fpsTimerStart(0), frameCount(0), currentFPS(0)
{}

bool Engine::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << "\n";
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

    // Load font (you need a .ttf file, place it in your project folder or provide full path)
    font = TTF_OpenFont("LCALLIG.ttf", 16);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << "\n";
        //SDL_Delay(5000); //FOR DEBUGGING
        return false;
    }

    fpsTimerStart = SDL_GetTicks();

    isRunning = true;
    return true;
}

void Engine::updateFPS() {
    frameCount++;
    Uint32 elapsed = SDL_GetTicks() - fpsTimerStart;

    if (elapsed >= 1000) {  // Every 1 second
        currentFPS = frameCount;
        frameCount = 0;
        fpsTimerStart = SDL_GetTicks();

        // Create new FPS texture
        if (fpsTexture) {
            SDL_DestroyTexture(fpsTexture);
            fpsTexture = nullptr;
        }

        std::stringstream ss;
        ss << "FPS: " << currentFPS;
        fpsTexture = createTextTexture(ss.str(), { 255, 255, 255, 255 });

        // Position top right
        if (fpsTexture) {
            int texW = 0;
            int texH = 0;
            SDL_QueryTexture(fpsTexture, NULL, NULL, &texW, &texH);
            fpsRect = { width - texW - 10, 10, texW, texH };
        }
    }
}

SDL_Texture* Engine::createTextTexture(const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Failed to create text surface! TTF_Error: " << TTF_GetError() << "\n";
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void Engine::renderFPS() {
    if (fpsTexture) {
        SDL_RenderCopy(renderer, fpsTexture, NULL, &fpsRect);
    }
}

void Engine::handleInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
        }
    }

    // Get current keyboard state
    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP]) {
        rectY -= rectSpeed;
    }
    if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN]) {
        rectY += rectSpeed;
    }
    if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT]) {
        rectX -= rectSpeed;
    }
    if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {
        rectX += rectSpeed;
    }
}


void Engine::run() {
    while (isRunning) {
        handleInput();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render();
        renderFPS();

        SDL_RenderPresent(renderer);

        updateFPS();

        SDL_Delay(16);  // ~60 FPS CHANGE TO CHANGE FPS (16 for 60 FPS)
    }
}

void Engine::render() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_Rect rect = { rectX, rectY, 200, 150 };
    SDL_RenderFillRect(renderer, &rect);
}

void Engine::cleanup() {
    if (fpsTexture) {
        SDL_DestroyTexture(fpsTexture);
        fpsTexture = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
