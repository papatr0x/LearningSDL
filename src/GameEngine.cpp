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

void GameEngine::init(const char* title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }
    m_window = SDL_CreateWindow(title,width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_METAL);
    if (!m_window) {
        std::cerr << "SDL could create window. Error: " << SDL_GetError() << "\n";
        return;
    }
    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_renderer) {
        std::cerr << "SDL could not create renderer. Error: " << SDL_GetError() << "\n";
    }
    SDL_SetRenderVSync(m_renderer, 1);

    SDL_SetRenderDrawColor(m_renderer, 0x1a, 0x2a, 0x3a, 0xff);
    srand(static_cast<unsigned>(time(0)));
    std::clog << "SDL Init succeeded\n";
    m_running = true;
    m_componentPool.reserve(256);
}

void GameEngine::render() {
    SDL_RenderClear(m_renderer);
    for (auto& [owner, comp] : m_componentPool) {
        if (owner->isActive() && comp->isEnabled())
            comp->render(m_renderer);
    }
    SDL_RenderPresent(m_renderer);
}

void GameEngine::update(float deltaTime) {
    for (auto& [owner, comp] : m_componentPool) {
        if (owner->isActive() && comp->isEnabled())
            comp->update(deltaTime, owner);
    }
}

void GameEngine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_running = false;
        }
    }
}

void GameEngine::shutdown() const {
    std::clog << "shutting down game\n";
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_Quit();
}