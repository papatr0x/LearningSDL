// Created by Patricio Palma on 28-02-26.

#include "GameEngine.h"
#include "MyScene.h"
#include <iostream>

int main() {
    GameEngine& engine = GameEngine::instance();
    if (!engine.init("SDL Game", 800, 600)) {
        std::cerr << "Cannot initialize tge game.\n";
        return EXIT_FAILURE;
    }
    engine.loadScene<MyScene>();
    engine.run();
    engine.shutdown();
    return EXIT_SUCCESS;
}
