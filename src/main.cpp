#include "Game.h"
#include <iostream>

int main() {
    auto game = std::make_unique<Game>();
    game->init("SDL Game", 800, 600);
    while (game->running()) {
        game->handleEvents();
        game->update();
        game->render();
    }
    game->shutdown();
    return EXIT_SUCCESS;
}