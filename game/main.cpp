// Created by Patricio Palma on 28-02-26.

#include "GameEngine.h"
#include "SceneOrchestrator.h"
#include "FirstScene.h"
#include "MenuScene.h"
#include "GameScene.h"
#include "IntermediateScene.h"
#include <iostream>

int main() {
    GameEngine& engine = GameEngine::instance();
    if (!engine.init("SDL Game", 800, 600)) {
        std::cerr << "Cannot initialize the game.\n";
        return EXIT_FAILURE;
    }

    auto& orch = SceneOrchestrator::instance();
    orch.registerScene<FirstScene>("first");
    orch.registerScene<MenuScene>("menu");
    orch.registerScene<GameScene>("game");
    orch.registerScene<IntermediateScene>("inter");

    // Transition from menu to game shows an intermediate scene first.
    orch.setTransition("menu", "game", "inter");

    orch.start("first");

    engine.run();
    engine.shutdown();
    return EXIT_SUCCESS;
}
