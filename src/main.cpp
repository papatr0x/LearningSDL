// Created by Patricio Palma on 28-02-26.

#include "GameEngine.h"
#include "MyScene.h"

int main() {
    GameEngine& engine = GameEngine::instance();
    engine.init("SDL Game", 800, 600);
    engine.loadScene<MyScene>();
    engine.run();
    engine.shutdown();
    return EXIT_SUCCESS;
}
