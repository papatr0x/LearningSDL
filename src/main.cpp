// Created by Patricio Palma on 28-02-26.

#include "GameEngine.h"
#include "RenderComponent.h"
#include "ScriptComponent.h"
#include "TextureManager.h"
#include <cmath>

#include "InputComponent.h"

void initGame() {
    GameEngine& engine = GameEngine::instance();
    TextureManager& texManager = TextureManager::instance();

    int screenWidth, screenHeight;
    SDL_GetWindowSize(GameEngine::instance().getWindow(), &screenWidth, &screenHeight);

    // --- Player ---
    auto player  = engine.addObjectToScene<Object>("Player");
    player->setTag("player");
    player->transform.position = {screenWidth/2.f, screenHeight/2.f};
    player->addComponent<InputComponent>("Player InputComponent", engine.getInput());
    auto playerRenderComp = player->addComponent<RenderComponent>("Player RenderComponent");
    playerRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-gold_112x112.png"));

    player->addComponent<ScriptComponent>("Player Script", [](float dt, Object* owner) mutable {
        auto* inputCompo = owner->getComponent<InputComponent>();
        if (!inputCompo) {
            SDL_Log("ScriptComponent::lambdaScript - InputComponent is null");
            return;
        }

        // Move to the mouse position
        if (inputCompo->isMouseButtonPressed(SDL_BUTTON_LEFT)) {
            owner->transform.position = inputCompo->getMousePosition();
        }
        // WASD movement
        constexpr float speed = 100.0f;
        if (inputCompo->isKeyDown(SDLK_W)) owner->transform.position.y -= speed * dt;
        if (inputCompo->isKeyDown(SDLK_S)) owner->transform.position.y += speed * dt;
        if (inputCompo->isKeyDown(SDLK_A)) owner->transform.position.x -= speed * dt;
        if (inputCompo->isKeyDown(SDLK_D)) owner->transform.position.x += speed * dt;
    });

    // --- Enemy ---
    auto enemy  = engine.addObjectToScene<Object>("Enemy");
    enemy->setTag("enemy");
    auto enemyRenderComp = enemy->addComponent<RenderComponent>("Enemy RenderComponent");
    enemyRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-pink_45x40.png"));
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
        const float deltaTime = static_cast<float>(currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        engine.handleEvents();
        engine.update(deltaTime);
        engine.render();
    }

    engine.shutdown();
    return EXIT_SUCCESS;
}
