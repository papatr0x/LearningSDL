// Created by Patricio Palma on 28-02-26.

#include "GameEngine.h"
#include "RenderComponent.h"
#include "SceneObject.h"
#include "ScriptComponent.h"
#include "TextureManager.h"
#include <cmath>

void initGame() {
    GameEngine& engine = GameEngine::instance();
    TextureManager& texManager = TextureManager::instance();

    // --- Player ---
    auto player  = engine.addObjectToScene<SceneObject>("Player");
    player->setTag("player");
    player->setTransform(Transform::make_area(112,112));

    auto playerRenderComp = player->addComponent<RenderComponent>("Player RenderComponent");
    playerRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-gold_112x112.png"));
    playerRenderComp->setTransformPtr(player->getTransform());

    float angle = 0.0f;
    player->addComponent<ScriptComponent>("Player Script", [angle, player](float dt, Object* /*owner*/) mutable {
        int w, h;
        SDL_GetWindowSize(GameEngine::instance().getWindow(), &w, &h);
        const float centerX = static_cast<float>(w) * 0.5f;
        const float centerY = static_cast<float>(h) * 0.5f;
        const float radius  = 100.0f;
        const float speed   = 1.0f;
        angle += speed * dt;
        player->setTransform(Transform::make_position(centerX + radius * std::cos(angle),
                                                      centerY + radius * std::sin(angle)));
    });

    // --- Enemy ---
    auto enemy  = engine.addObjectToScene<SceneObject>("Enemy");
    enemy->setTag("enemy");
    enemy->setTransform(Transform::make_area(45,40));

    auto enemyRenderComp = enemy->addComponent<RenderComponent>("Enemy RenderComponent");
    enemyRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-pink_45x40.png"));
    enemyRenderComp->setTransformPtr(enemy->getTransform());
    enemy->setTransform(Transform::make_position(100,100));


    enemy->addComponent<ScriptComponent>("Enemy Script", [enemy](float dt, Object* /*owner*/) {
        float rand1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float rand2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        const float speed = 200.f;
        auto* enemyTransform = enemy->getTransform();
        enemyTransform->x += (rand1 - 0.5f) * speed * dt;
        enemyTransform->y += (rand2 - 0.5f) * speed * dt;
    });
}

int main() {
    GameEngine& engine = GameEngine::instance();
    engine.init("SDL Game", 800, 600);

    initGame();

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
