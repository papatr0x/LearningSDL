// Created by Patricio Palma on 28-02-26.

#include "Game.h"
#include <iostream>

int main() {
    auto game = std::make_unique<Game>();
    game->init("SDL Game", 800, 600);

    uint64_t lastTime = SDL_GetTicks();

    while (game->running()) {
        uint64_t currentTime = SDL_GetTicks();
        float deltaTime = static_cast<float>(currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        game->handleEvents();
        game->update(deltaTime);
        game->render();
    }
    game->shutdown();
    return EXIT_SUCCESS;
}