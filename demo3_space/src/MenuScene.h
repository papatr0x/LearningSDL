// Created by Patricio Palma on 20-03-26.

#ifndef LETSLEARNSDL_PONG_MENUSCENE_H
#define LETSLEARNSDL_PONG_MENUSCENE_H

#include "BuildInfo.h"
#include "FontManager.h"
#include "GameEngine.h"
#include "InputComponent.h"
#include "Scene.h"
#include "ScriptComponent.h"
#include "TextComponent.h"
#include "TransitionComponent.h"
#include <string>

class MenuScene : public Scene {
public:
    void load() override {
        setBackgroundColor({0x1a, 0x1a, 0x2e, 0xff});

        auto& [sw, sh] = getScreenSize();

        TTF_Font* fontBig = FontManager::instance().load("assets/BitcountSingle-Regular.ttf", 64.f);
        TTF_Font* fontMed = FontManager::instance().load("assets/Roboto-Regular.ttf", 24.f);

        const float cx = sw * 0.5f;
        const float cy = sh * 0.5f;

        auto ui = addObject("UI");
        ui->transform.position = {cx, cy};

        auto* title = ui->addComponent<TextComponent>("Title", "Space Invaders", fontBig, SDL_Color{0x0, 0xff, 0x0, 0xff});
        title->setOffset({0.f, -80.f});

        auto subtitle = ui->addComponent<TextComponent>("SubTitle", "[WORK IN PROGRESS]", fontMed);
        subtitle->setOffset({0.f, -40.f});

        auto* play = ui->addComponent<TextComponent>("Play", "[ENTER] Play", fontMed);
        play->setOffset({0.f, 20.f});

        auto* quit = ui->addComponent<TextComponent>("Quit", "[ESC] Quit", fontMed);
        quit->setOffset({0.f, 60.f});

        TTF_Font* fontSmall = FontManager::instance().load("assets/Roboto-Regular.ttf", 13.f);
        auto* buildObj = addObject("BuildVersion");
        buildObj->transform.position = {cx, sh - 15.f};
        buildObj->addComponent<TextComponent>("Text", std::string{"build "} + kBuildVersion, fontSmall);

        // --- Input ---
        auto inputObj = addObject("Input");
        auto inputComp = inputObj->addComponent<InputComponent>("MenuInput");
        auto tc = inputObj->addComponent<TransitionComponent>("ToPong", "thegame");

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
