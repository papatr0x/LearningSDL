// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_MENUSCENE_H
#define LETSLEARNSDL_MENUSCENE_H

#include "Scene.h"
#include "GameEngine.h"
#include "FontManager.h"
#include "TextComponent.h"
#include "InputComponent.h"
#include "ScriptComponent.h"
#include "TransitionComponent.h"

class MenuScene : public Scene {
public:
    void load() override {
        int screenWidth, screenHeight;
        SDL_GetWindowSize(GameEngine::instance().getWindow(), &screenWidth, &screenHeight);
        TTF_Font* font = FontManager::instance().load("assets/Roboto-Regular.ttf", 24.f);

        const float cx = screenWidth / 2.f;
        const float cy = screenHeight / 2.f;

        auto* ui = addObject<Object>("UI");
        ui->transform.position = {cx, cy};
        ui->addComponent<TextComponent>("Title", "MAIN MENU", font);
        auto opt1 = ui->addComponent<TextComponent>("Opt1", "[1] Splash screen", font);
        opt1->setOffset({0.f, 40.f});
        auto opt2 = ui->addComponent<TextComponent>("Opt2", "[2] Game", font);
        opt2->setOffset({0.f, 80.f});
        auto opt3 = ui->addComponent<TextComponent>("Opt3", "[ESC] Exit", font);
        opt3->setOffset({0.f, 120.f});

        auto inputObject = addObject<Object>("Input");
        auto inputCompo = inputObject->addComponent<InputComponent>("MenuInput");
        auto tc1 = inputObject->addComponent<TransitionComponent>("ToFirst", "first");
        auto tc2 = inputObject->addComponent<TransitionComponent>("ToGame",  "game");

        inputObject->addComponent<ScriptComponent>("MenuScript", [tc1, tc2, inputCompo](float, Object* owner) {
            if (inputCompo->isKeyPressed(SDL_SCANCODE_1))
                tc1->trigger();
            if (inputCompo->isKeyPressed(SDL_SCANCODE_2))
                tc2->trigger();
            if (inputCompo->isKeyPressed(SDL_SCANCODE_ESCAPE))
                GameEngine::instance().quit();
        });
    }
};

#endif //LETSLEARNSDL_MENUSCENE_H
