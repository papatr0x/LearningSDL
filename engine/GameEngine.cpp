// Created by Patricio Palma on 28-02-26.

#include "GameEngine.h"
#include "AudioManager.h"
#include "FontManager.h"
#include "TextureManager.h"
#include <iostream>
#include <memory>
#include <algorithm>

GameEngine& GameEngine::instance() {
    static GameEngine instance;
    return instance;
}

bool GameEngine::init(const char* title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
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

    if (!TTF_Init()) {
        std::cerr << "SDL_ttf could not initialize! Error: " << SDL_GetError() << "\n";
        return false;
    }
    textEngine = TTF_CreateRendererTextEngine(renderer);
    if (!textEngine) {
        std::cerr << "TTF_CreateRendererTextEngine failed: " << SDL_GetError() << "\n";
        return false;
    }

    if (!AudioManager::instance().init()) return false;

    SDL_SetRenderDrawColor(renderer, 0x1a, 0x2a, 0x3a, 0xff);
    srand(static_cast<unsigned>(time(nullptr)));
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
    activeScene.reset();                     // destroy all components (TTF_Text*, SDL_Texture*, MIX_Track*)
    AudioManager::instance().unloadAll();    // stop and free all audio before SDL_Quit
    FontManager::instance().unloadAll();     // close all TTF_Font* before destroying the engine
    TTF_DestroyRendererTextEngine(textEngine);
    TextureManager::instance().unloadAll();  // destroy SDL_Texture* before renderer goes down
    TTF_Quit();
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void GameEngine::run() {
    uint64_t lastTime = SDL_GetTicks();
    while (running) {
        // Generate deltatime
        const uint64_t currentTime = SDL_GetTicks();
        const float deltaTime = std::min(static_cast<float>(currentTime - lastTime) / 1000.0f, maxDeltaTime);
        lastTime = currentTime;

        // Check inputs
        handleEvents();

        // Handle scene transitions
        if (pendingScene) {
            if (activeScene)
                activeScene->unload();
            activeScene.reset();
            activeScene = std::move(pendingScene);
            activeScene->start();
        }
        if (!activeScene) continue;

        // Update and render the current scene
        activeScene->update(deltaTime);
        activeScene->render(renderer);
    }
}
