// Created by Patricio Palma on 28-02-26.

#include "Game.h"
#include "TextureManager.h"
#include <iostream>

void Game::init(const char* title, int width, int height) {
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

    std::clog << "SDL Init succeeded\n";
    m_running = true;

    // Game init
    TextureManager::instance().load("assets/fish-45x40.png", "fish", m_renderer);

    m_go.load(100, 100, 45, 40, "fish");
    m_player.load(300, 300, 45, 40, "fish");
}

void Game::render() {
    SDL_RenderClear(m_renderer);
    m_go.draw(m_renderer);
    m_player.draw(m_renderer);
    SDL_RenderPresent(m_renderer);
}

void Game::update(float deltaTime) {
    m_go.update(deltaTime);
    m_player.update(deltaTime);
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_running = false;
        }
    }
}

void Game::shutdown() const {
    std::clog << "shutting down game\n";
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_Quit();
}
