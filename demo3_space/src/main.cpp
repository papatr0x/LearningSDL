// Created by Patricio Palma on 20-03-26.

#include "GameEngine.h"
#include "SceneOrchestrator.h"
#include "MenuScene.h"
#include "SpaceInvadersScene.h"
#include "GameOverScene.h"
#include "GameStateManager.h"
#include "GameStats.h"

#include <iostream>

int main() {
    GameEngine& engine = GameEngine::instance();
    if (!engine.init("Space Invaders++", 600, 800)) {
        std::cerr << "Cannot initialize the game.\n";
        return EXIT_FAILURE;
    }

    GameStateManager::instance().set(GameStats{});


    auto& orch = SceneOrchestrator::instance();
    orch.registerScene<MenuScene>("menu");
    orch.registerScene<SpaceInvadersScene>("thegame");
    orch.registerScene<GameOverScene>("gameover");
    orch.start("menu");

    engine.run();
    engine.shutdown();
    return EXIT_SUCCESS;
}
