// Created by Patricio Palma on 28-02-26.

#include "GameEngine.h"
#include "TextureManager.h"
#include <iostream>
#include <memory>
#include <algorithm>

GameEngine& GameEngine::instance() {
    static GameEngine instance;
    return instance;
}

bool GameEngine::init(const char* title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }
    window = SDL_CreateWindow(title,width, height, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL could create window. Error: " << SDL_GetError() << "\n";
        return false;
    }
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL could not create renderer. Error: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_SetRenderVSync(renderer, 1);

    SDL_SetRenderDrawColor(renderer, 0x1a, 0x2a, 0x3a, 0xff);
    srand(static_cast<unsigned>(time(0)));
    running = true;

    std::clog << "SDL Init succeeded\n";
    return true;
}

void GameEngine::handleEvents() {
    input.beginFrame();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) running = false;
        input.processEvent(event);
    }
}

void GameEngine::shutdown() {
    std::clog << "shutting down game\n";
    if (activeScene) activeScene->unload();
    TextureManager::instance().unloadAll(); // destroy textures before SDL goes down
    if (window) SDL_DestroyWindow(window);
    if (renderer) SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void GameEngine::run() {
    uint64_t lastTime = SDL_GetTicks();
    while (running) {
        // Generate deltatime
        const uint64_t currentTime = SDL_GetTicks();
        const float deltaTime = static_cast<float>(currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Check inputs
        handleEvents();

        // Handle scene transitions
        if (pendingScene) {
            if (activeScene)
                activeScene->unload();
            activeScene = std::move(pendingScene);
            activeScene->start();
        }
        if (!activeScene) continue;

        // Update and render the current scene
        activeScene->update(deltaTime);
        activeScene->render(renderer);
    }
}
