// Created by Patricio Palma on 07-03-26.

#ifndef LETSLEARNSDL_MYSCENE_H
#define LETSLEARNSDL_MYSCENE_H

#include "Scene.h"
#include "GameEngine.h"
#include "InputComponent.h"
#include "RenderComponent.h"
#include "ScriptComponent.h"
#include "TextureManager.h"

class MyScene : public Scene {
public:
    void load() override {
        GameEngine& engine = GameEngine::instance();
        TextureManager& texManager = TextureManager::instance();

        int screenWidth, screenHeight;
        SDL_GetWindowSize(GameEngine::instance().getWindow(), &screenWidth, &screenHeight);

        // --- Player ---
        auto player  = addObject<Object>("Player");
        player->setTag("player");
        player->transform.position = {screenWidth/2.f, screenHeight/2.f};
        auto* inputComp = player->addComponent<InputComponent>("Player InputComponent", engine.getInput());
        inputComp->bind("MoveUp",    SDL_SCANCODE_W);
        inputComp->bind("MoveDown",  SDL_SCANCODE_S);
        inputComp->bind("MoveLeft",  SDL_SCANCODE_A);
        inputComp->bind("MoveRight", SDL_SCANCODE_D);

        auto playerRenderComp = player->addComponent<RenderComponent>("Player RenderComponent");
        playerRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-gold_112x112.png"));

        player->addComponent<ScriptComponent>("Player Script", [](float dt, Object* owner) {
            auto* inputCompo = owner->getComponent<InputComponent>();
            if (!inputCompo) {
                SDL_Log("ScriptComponent::lambdaScript - InputComponent is null");
                return;
            }

            // Move to the mouse position
            if (inputCompo->isMouseButtonPressed(SDL_BUTTON_LEFT)) {
                owner->transform.position = inputCompo->getMousePosition();
            }
            constexpr float speed = 100.0f;

            // WASD movement
            if (inputCompo->isActionDown("MoveUp"))    owner->transform.position.y -= speed * dt;
            if (inputCompo->isActionDown("MoveDown"))  owner->transform.position.y += speed * dt;
            if (inputCompo->isActionDown("MoveLeft"))  owner->transform.position.x -= speed * dt;
            if (inputCompo->isActionDown("MoveRight")) owner->transform.position.x += speed * dt;

            // Left analog stick
            owner->transform.position.x += inputCompo->getAxis(SDL_GAMEPAD_AXIS_LEFTX) * speed * dt;
            owner->transform.position.y += inputCompo->getAxis(SDL_GAMEPAD_AXIS_LEFTY) * speed * dt;
        });

        // --- Enemy ---
        auto enemy  = addObject<Object>("Enemy");
        enemy->setTag("enemy");
        auto enemyRenderComp = enemy->addComponent<RenderComponent>("Enemy RenderComponent");
        enemyRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-pink_45x40.png"));
        enemy->transform = Transform::make_position(screenWidth/2,screenHeight/2);

        enemy->addComponent<ScriptComponent>("Enemy Script",
            [width=static_cast<float>(screenWidth), height=static_cast<float>(screenHeight),
             dirX = (rand() > RAND_MAX/2) ? 1.f : -1.f,
             dirY = (rand() > RAND_MAX/2) ? 1.f : -1.f]
            (float dt, Object* enemy) mutable {
                constexpr float speed = 100.f;
                Vec2F& pos = enemy->transform.position;
                pos.x += dirX * speed * dt;
                pos.y += dirY * speed * dt;
                if (pos.x > width || pos.x < 0.f)  dirX = -dirX;
                if (pos.y > height || pos.y < 0.f) dirY = -dirY;
        });
    }
};

#endif //LETSLEARNSDL_MYSCENE_H