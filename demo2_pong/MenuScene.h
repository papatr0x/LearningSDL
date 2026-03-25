// Created by Patricio Palma on 20-03-26.

#ifndef LETSLEARNSDL_PONG_MENUSCENE_H
#define LETSLEARNSDL_PONG_MENUSCENE_H

#include "FontManager.h"
#include "GameEngine.h"
#include "InputComponent.h"
#include "Scene.h"
#include "ScriptComponent.h"
#include "TextComponent.h"
#include "TransitionComponent.h"

class MenuScene : public Scene {
public:
    void load() override {
        backgroundColor = {0x1a, 0x1a, 0x2e, 0xff};

        auto& [sw, sh] = getScreenSize();

        TTF_Font* fontBig = FontManager::instance().load("assets/Roboto-Regular.ttf", 64.f);
        TTF_Font* fontMed = FontManager::instance().load("assets/Roboto-Regular.ttf", 24.f);

        const float cx = sw * 0.5f;
        const float cy = sh * 0.5f;

        auto* ui = addObject<Object>("UI");
        ui->transform.position = {cx, cy};

        auto* title = ui->addComponent<TextComponent>("Title", "PONG", fontBig);
        title->setOffset({0.f, -80.f});

        auto* play = ui->addComponent<TextComponent>("Play", "[ENTER] Play", fontMed);
        play->setOffset({0.f, 20.f});

        auto* quit = ui->addComponent<TextComponent>("Quit", "[ESC] Quit", fontMed);
        quit->setOffset({0.f, 60.f});

        // --- Input ---
        auto* inputObj  = addObject<Object>("Input");
        auto* inputComp = inputObj->addComponent<InputComponent>("MenuInput");
        auto* tc        = inputObj->addComponent<TransitionComponent>("ToPong", "pong");

        inputObj->addComponent<ScriptComponent>("MenuScript", [tc, inputComp](float, Object*) {
            if (inputComp->isKeyPressed(SDL_SCANCODE_RETURN) ||
                inputComp->isKeyPressed(SDL_SCANCODE_KP_ENTER))
                tc->trigger();

            if (inputComp->isKeyPressed(SDL_SCANCODE_ESCAPE))
                GameEngine::instance().quit();
        });
    }
};

#endif //LETSLEARNSDL_PONG_MENUSCENE_H
