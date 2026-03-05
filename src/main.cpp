// Created by Patricio Palma on 28-02-26.

#include "Enemy.h"
#include "GameEngine.h"
#include "Player.h"

void initGame() {
    GameEngine& engine = GameEngine::instance();
    Object* player = engine.addObjectToScene<Player>("Player");
    Object* enemy = engine.addObjectToScene<Enemy>("Enemy");
}

int main() {
    GameEngine& engine = GameEngine::instance();
    engine.engineInit("SDL Game", 800, 600);

    uint64_t lastTime = SDL_GetTicks();

    while (engine.running()) {
        uint64_t currentTime = SDL_GetTicks();
        float deltaTime = static_cast<float>(currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        engine.handleEvents();
        engine.update(deltaTime);
        engine.render();
    }
    engine.shutdown();
    return EXIT_SUCCESS;
}