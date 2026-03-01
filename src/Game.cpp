// Created by Patricio Palma on 28-02-26.

#include "Game.h"
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

    SDL_SetRenderDrawColor(m_renderer, 0x0a, 0x1a, 0x2a, 0xff);

    std::clog << "SDL Init succeeded\n";
    m_running = true;
}

void Game::render() {
    SDL_RenderClear(m_renderer);
    SDL_RenderPresent(m_renderer);
}

void Game::update() {
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_running = false;
        }
    }
}

void Game::shutdown() {
    std::clog << "shutting down game\n";
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_Quit();
}
