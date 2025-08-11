#include "Engine.h"
#include <iostream>
#include <sstream>
#include <SDL_image.h>

Engine::Engine(const std::string& title, int width, int height)
    : title(title), width(width), height(height),
      window(nullptr), renderer(nullptr), isRunning(false),
      rectX(100), rectY(100), rectSpeed(4),
      font(nullptr), fpsTexture(nullptr), fpsTimerStart(0), frameCount(0), currentFPS(0),
      currentState(GameState::TITLE_SCREEN)
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
    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << "\n";
        return false;
    }

    playerTexture = IMG_LoadTexture(renderer, "assets/characters/LAMBDAPLAYERTEST.png");
    if (!playerTexture) {
        std::cerr << "Failed to load player sprite: " << IMG_GetError() << "\n";
        return false;
    }

    SDL_SetTextureColorMod(playerTexture, 255, 255, 255);
    SDL_SetTextureAlphaMod(playerTexture, 255);

    // Load font (you need a .ttf file, place it in your project folder or provide full path)
    font = TTF_OpenFont("LCALLIG.ttf", 16);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << "\n";
        //SDL_Delay(5000); //FOR DEBUGGING
        return false;
    }

    SDL_Surface* heartSurface = IMG_Load("assets/menusprites/HEALTH.png");
    if (!heartSurface) {
        SDL_Log("Failed to load heart: %s", IMG_GetError());
        return false;
    }
    heartTexture = SDL_CreateTextureFromSurface(renderer, heartSurface);
    SDL_FreeSurface(heartSurface);

    SDL_Surface* bombSurface = IMG_Load("assets/menusprites/BOMB.png");
    if (!bombSurface) {
        SDL_Log("Failed to load bomb: %s", IMG_GetError());
        return false;
    }
    bombTexture = SDL_CreateTextureFromSurface(renderer, bombSurface);
    SDL_FreeSurface(bombSurface);


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

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    // Detect ESC key press to toggle pause
    static bool escPreviouslyDown = false;
    bool escDown = keystates[SDL_SCANCODE_ESCAPE];
    if (escDown && !escPreviouslyDown) {
        if (currentState == GameState::GAME_RUNNING) {
            currentState = GameState::PAUSED;
        }
        else if (currentState == GameState::PAUSED) {
            currentState = GameState::GAME_RUNNING;
        }
    }
    escPreviouslyDown = escDown;

    if (currentState == GameState::PAUSED) {
        // Only allow pause menu navigation inputs here

        static bool zPreviouslyDown = false;
        bool zDown = keystates[SDL_SCANCODE_Z];
        bool upDown = keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP];
        bool downDown = keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN];

        static bool upPreviouslyDown = false;
        static bool downPreviouslyDown = false;

        // Navigate pause menu up
        if (upDown && !upPreviouslyDown) {
            pauseMenuSelection--;
            if (pauseMenuSelection < 0) pauseMenuSelection = 1; // wrap around
        }
        upPreviouslyDown = upDown;

        // Navigate pause menu down
        if (downDown && !downPreviouslyDown) {
            pauseMenuSelection++;
            if (pauseMenuSelection > 1) pauseMenuSelection = 0; // wrap around
        }
        downPreviouslyDown = downDown;

        // Select option
        if (zDown && !zPreviouslyDown) {
            if (pauseMenuSelection == 0) {
                // Title selected
                currentState = GameState::TITLE_SCREEN;
            } else if (pauseMenuSelection == 1) {
                // Continue selected
                currentState = GameState::GAME_RUNNING;
            }
        }
        zPreviouslyDown = zDown;

        // Do not process any other input while paused
        return;
    }

    // GAME_RUNNING state input handling below

    int currentSpeed = rectSpeed;
    if (keystates[SDL_SCANCODE_LSHIFT]) {
        currentSpeed = rectSpeed / 2;
    }
    if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP]) {
        rectY -= currentSpeed;
    }
    if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN]) {
        rectY += currentSpeed;
    }
    if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT]) {
        rectX -= currentSpeed;
    }
    if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {
        rectX += currentSpeed;
    }

    // Bomb key handling (X key)
    static bool xKeyPreviouslyDown = false;
    bool xKeyDown = keystates[SDL_SCANCODE_X];
    if (xKeyDown && !xKeyPreviouslyDown) {  // Just pressed
        if (bombs > 0) {
            bombs--;
            std::cout << "Bomb used! Remaining: " << bombs << std::endl;
            // TODO: trigger bomb effect here
        }
    }
    xKeyPreviouslyDown = xKeyDown;

    clampPosition();
}


void Engine::handleTitleInput(SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        isRunning = false;
    }
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_RETURN) {
            // Start the game
            currentState = GameState::GAME_RUNNING;

            // Set player position to middle horizontally in game area:
            // The game area width is width / 2
            rectX = (width / 2) / 2; // middle of left half

            // Set player Y to about 1/3 from bottom (height - 1/3 height)
            rectY = static_cast<int>(height * 2 / 3); 

            clampPosition();  // clamp in case edges are exceeded
        }
        else if (e.key.keysym.sym == SDLK_ESCAPE) {
            isRunning = false;
        }
    }
}

void Engine::clampPosition() {
    // Keep rectangle within game area with 10px walls
    int leftLimit = 10;
    int topLimit = 10;
    int rightLimit = (width / 2) - 10; // leave room before divider
    int bottomLimit = height - 10;

    if (rectX < leftLimit) rectX = leftLimit;
    if (rectY < topLimit) rectY = topLimit;
    if (rectX + hurtboxSize > rightLimit) rectX = rightLimit - hurtboxSize;
    if (rectY + hurtboxSize > bottomLimit) rectY = bottomLimit - hurtboxSize;
}

void Engine::run() {
    SDL_Event e;

    while (isRunning) {
        // Poll and handle all events
        while (SDL_PollEvent(&e)) {
            if (currentState == GameState::TITLE_SCREEN) {
                handleTitleInput(e);
            } else if (e.type == SDL_QUIT) {
                isRunning = false;
            }
            // You can add other event-based input handling here if needed
        }

        // After processing all events, update movement based on key state
        if (currentState == GameState::GAME_RUNNING) {
            handleInput();  // This uses SDL_GetKeyboardState and moves smoothly
        }

        // Rendering
        if (currentState == GameState::TITLE_SCREEN) {
            renderTitleScreen();
        } else if (currentState == GameState::GAME_RUNNING) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            render();
            renderFPS();
        }

        SDL_RenderPresent(renderer);

        updateFPS();

        SDL_Delay(16);  // ~60 FPS
    }
}

void Engine::drawFilledCircle(int centerX, int centerY, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset from center
            int dy = radius - h; // vertical offset from center
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void Engine::render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw walls for game area
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white walls

    // Top wall
    SDL_Rect topWall = { 0, 0, width / 2, 10 };
    SDL_RenderFillRect(renderer, &topWall);

    // Left wall
    SDL_Rect leftWall = { 0, 0, 10, height };
    SDL_RenderFillRect(renderer, &leftWall);

    // Bottom wall
    SDL_Rect bottomWall = { 0, height - 10, width / 2, 10 };
    SDL_RenderFillRect(renderer, &bottomWall);

    // Divider between game & menu
    SDL_Rect divider = { width / 2 - 10, 0, 10, height }; // 10px wide
    SDL_RenderFillRect(renderer, &divider);

    // Draw "HP:" text in menu
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* hpSurface = TTF_RenderText_Solid(font, "HP:", white);
    SDL_Texture* hpText = SDL_CreateTextureFromSurface(renderer, hpSurface);

    SDL_Rect hpTextRect = { width/2 + 20, 100, hpSurface->w, hpSurface->h };
    SDL_RenderCopy(renderer, hpText, NULL, &hpTextRect);

    SDL_FreeSurface(hpSurface);
    SDL_DestroyTexture(hpText);

    // Draw heart sprites next to HP:
    int heartWidth = 24;
    int heartHeight = 24;
    int heartX = hpTextRect.x + hpTextRect.w + 10; // offset after "HP:"
    int heartY = 100; // align with text

    for (int i = 0; i < playerHealth; i++) {
        SDL_Rect heartRect = { heartX + i * (heartWidth + 5), heartY, heartWidth, heartHeight };
        SDL_RenderCopy(renderer, heartTexture, NULL, &heartRect);
    }

    // --- Render Bomb Bar ---
    int bombX = 340; 
    int bombY = heartY + heartHeight + 10; // position below hearts
    int bombWidth = 32;
    int bombHeight = 32;
    int bombSpacing = 5;

    // Draw label "Bombs:"
    SDL_Surface* bombTextSurface = TTF_RenderText_Solid(font, "Bombs:", white);
    SDL_Texture* bombTextTexture = SDL_CreateTextureFromSurface(renderer, bombTextSurface);
    SDL_Rect bombTextRect = {bombX, bombY, bombTextSurface->w, bombTextSurface->h};
    SDL_RenderCopy(renderer, bombTextTexture, NULL, &bombTextRect);
    SDL_FreeSurface(bombTextSurface);
    SDL_DestroyTexture(bombTextTexture);

    // Draw bombs
    for (int i = 0; i < bombs; i++) {
        SDL_Rect destRect = {bombX + 80 + i * (bombWidth + bombSpacing), bombY, bombWidth, bombHeight};
        SDL_RenderCopy(renderer, bombTexture, NULL, &destRect);
    }

    int spriteWidth = 32;
    int spriteHeight = 64;
    int hurtboxSize = 5;
    int yOffset = 12;  // positive moves hurtbox up inside sprite
    int xOffset = 2;

    SDL_Rect dest = {
        rectX + hurtboxSize / 2 - spriteWidth / 2 + xOffset,
        rectY + hurtboxSize / 2 - spriteHeight / 2 + yOffset,
        spriteWidth,
        spriteHeight
    };

    SDL_RenderCopy(renderer, playerTexture, NULL, &dest);


    // Draw hurtbox for debugging (optional)
    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    //SDL_Rect hurtbox = { rectX, rectY, hurtboxSize, hurtboxSize };
    //SDL_RenderFillRect(renderer, &hurtbox);
    // Draw hurtbox as layered circles instead of red rectangle
    // Draw hurtbox only if left shift is pressed
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_LSHIFT]) {
        int cx = rectX + hurtboxSize / 2;
        int cy = rectY + hurtboxSize / 2;

        drawFilledCircle(cx, cy, 5, {0, 0, 139, 255});    // Dark blue outer circle
        drawFilledCircle(cx, cy, 3, {173, 216, 230, 255}); // Light blue middle circle
        drawFilledCircle(cx, cy, 1, {255, 255, 255, 255}); // White center circle
    }
}

void Engine::renderTitleScreen() {
    // Optional: clear with different background color for title screen
    SDL_SetRenderDrawColor(renderer, 30, 30, 60, 255);
    SDL_RenderClear(renderer);

    SDL_Color white = { 255, 255, 255, 255 };

    SDL_Texture* titleTexture = createTextTexture("My SDL2 Game", white);
    SDL_Texture* startTexture = createTextTexture("Press Enter to Start", white);
    SDL_Texture* exitTexture = createTextTexture("Press ESC to Quit", white);

    if (titleTexture && startTexture && exitTexture) {
        int tw, th, sw, sh, ew, eh;
        SDL_QueryTexture(titleTexture, NULL, NULL, &tw, &th);
        SDL_QueryTexture(startTexture, NULL, NULL, &sw, &sh);
        SDL_QueryTexture(exitTexture, NULL, NULL, &ew, &eh);

        SDL_Rect titleRect = { (width - tw) / 2, height / 4, tw, th };
        SDL_Rect startRect = { (width - sw) / 2, height / 2, sw, sh };
        SDL_Rect exitRect = { (width - ew) / 2, height / 2 + 40, ew, eh };

        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        SDL_RenderCopy(renderer, startTexture, NULL, &startRect);
        SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);
    }

    if (titleTexture) SDL_DestroyTexture(titleTexture);
    if (startTexture) SDL_DestroyTexture(startTexture);
    if (exitTexture) SDL_DestroyTexture(exitTexture);
}

void Engine::cleanup() {
    if (fpsTexture) {
        SDL_DestroyTexture(fpsTexture);
        fpsTexture = nullptr;
    }
    if (playerTexture) {
        SDL_DestroyTexture(playerTexture);
        playerTexture = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (bombTexture) {
        SDL_DestroyTexture(bombTexture);
        bombTexture = nullptr;
    }
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_DestroyTexture(heartTexture);
    TTF_Quit();
    SDL_Quit();
    IMG_Quit();
}
