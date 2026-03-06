// Created by Patricio Palma on 28-02-26.

#include "GameEngine.h"
#include "RenderComponent.h"
#include "ScriptComponent.h"
#include "TextureManager.h"
#include <cmath>

void initGame() {
    GameEngine& engine = GameEngine::instance();
    TextureManager& texManager = TextureManager::instance();

    // --- Player ---
    auto player  = engine.addObjectToScene<Object>("Player");
    player->setTag("player");
    auto playerRenderComp = player->addComponent<RenderComponent>("Player RenderComponent");
    playerRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-gold_112x112.png"));
    float angle = 0.0f;
    player->addComponent<ScriptComponent>("Player Script", [angle](float dt, Object* player) mutable {
        int w, h;
        SDL_GetWindowSize(GameEngine::instance().getWindow(), &w, &h);
        const float centerX = static_cast<float>(w) * 0.5f;
        const float centerY = static_cast<float>(h) * 0.5f;
        constexpr float radius  = 200.0f;
        constexpr float speed   = 1.0f;
        angle += speed * dt;
        player->transform.position.x = centerX + radius * std::cos(angle) - 56;
        player->transform.position.y = centerY + radius * std::sin(angle) - 56;
    });

    // --- Enemy ---
    auto enemy  = engine.addObjectToScene<Object>("Enemy");
    enemy->setTag("enemy");
    auto enemyRenderComp = enemy->addComponent<RenderComponent>("Enemy RenderComponent");
    enemyRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-pink_45x40.png"));
    int screenWidth, screenHeight;
    SDL_GetWindowSize(GameEngine::instance().getWindow(), &screenWidth, &screenHeight);
    enemy->transform = Transform::make_position(screenWidth/2,screenHeight/2);

    enemy->addComponent<ScriptComponent>("Enemy Script",
            [width=(float)screenWidth, height=(float)screenHeight] (float dt, Object* enemy) {
                static float dirX = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) > 0.0f ? 1.f : -1.f;
                static float dirY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f > 0.0f ? 1.f : -1.f;

                constexpr float speed = 100.f;
                Vec2F& enemyPosition = enemy->transform.position;
                enemyPosition.x += dirX * speed * dt;
                enemyPosition.y += dirY * speed * dt;
                if (enemyPosition.x > width || enemyPosition.x < 0.f)
                    dirX = -dirX;
                if (enemyPosition.y > height || enemyPosition.y < 0.f)
                    dirY = -dirY;
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
