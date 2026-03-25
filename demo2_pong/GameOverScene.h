// Created by Patricio Palma on 20-03-26.

#ifndef LETSLEARNSDL_GAMEOVERSCENE_H
#define LETSLEARNSDL_GAMEOVERSCENE_H

#include "FontManager.h"
#include "GameEngine.h"
#include "InputComponent.h"
#include "PongState.h"
#include "Scene.h"
#include "ScriptComponent.h"
#include "TextComponent.h"
#include "TransitionComponent.h"
#include <string>

class GameOverScene : public Scene {
public:
    void load() override {
        setBackgroundColor({0x1a, 0x1a, 0x2e, 0xff});

        auto& [sw, sh] = getScreenSize();

        TTF_Font* fontBig = FontManager::instance().load("assets/Roboto-Regular.ttf", 64.f);
        TTF_Font* fontMed = FontManager::instance().load("assets/Roboto-Regular.ttf", 36.f);
        TTF_Font* fontSml = FontManager::instance().load("assets/Roboto-Regular.ttf", 24.f);

        const std::string winnerText =
            (PongState::winner == 1) ? "PLAYER WINS!" : "AI WINS!";
        const std::string scoreText =
            std::to_string(PongState::scoreP1) + "  -  " + std::to_string(PongState::scoreP2);

        auto ui = addObject("UI");
        ui->transform.position = {sw * 0.5f, sh * 0.5f};

        auto winner = ui->addComponent<TextComponent>("Winner", winnerText, fontBig);
        winner->setOffset({0.f, -80.f});

        auto score = ui->addComponent<TextComponent>("Score", scoreText, fontMed);
        score->setOffset({0.f, 20.f});

        auto hint = ui->addComponent<TextComponent>("Hint", "[ENTER] Back to Menu", fontSml);
        hint->setOffset({0.f, 100.f});

        // --- Input ---
        auto inputObj = addObject("Input");
        auto inputComp = inputObj->addComponent<InputComponent>("GameOverInput");
        auto tc = inputObj->addComponent<TransitionComponent>("ToMenu", "menu");

        inputObj->addComponent<ScriptComponent>("InputScript", [tc, inputComp](float, Object*) {
            if (inputComp->isKeyPressed(SDL_SCANCODE_RETURN) ||
                inputComp->isKeyPressed(SDL_SCANCODE_KP_ENTER))
                tc->trigger();
        });
    }
};

#endif //LETSLEARNSDL_GAMEOVERSCENE_H
