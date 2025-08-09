#pragma once
#include <SDL.h>
#include <SDL_ttf.h>      // Add this
#include <string>

enum class GameState {
    TITLE_SCREEN,
    GAME_RUNNING
};
class Engine {
public:
    Engine(const std::string& title, int width, int height);
    bool init();
    void run();
    void cleanup();
    void render();
    void handleInput();
    void clampPosition();

private:
    std::string title;
    int width;
    int height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    GameState currentState;

    int rectX;
    int rectY;
    int rectSpeed;

    // For FPS counter
    TTF_Font* font = nullptr;
    SDL_Texture* fpsTexture = nullptr;
    SDL_Rect fpsRect;
    Uint32 fpsTimerStart = 0;
    int frameCount = 0;
    int currentFPS = 0;

    void updateFPS();
    void renderFPS();
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);

    void renderTitleScreen();
    void handleTitleInput(SDL_Event& e);
};
