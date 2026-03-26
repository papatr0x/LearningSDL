// Created by Patricio Palma on 20-03-26.

#ifndef LETSLEARNSDL_GAMEOVERSCENE_H
#define LETSLEARNSDL_GAMEOVERSCENE_H

#include "FontManager.h"
#include "InputComponent.h"
#include "Scene.h"
#include "ScriptComponent.h"
#include "TextComponent.h"
#include "TransitionComponent.h"

class GameOverScene : public Scene {
public:
    void load() override {
        setBackgroundColor({0x1a, 0x1a, 0x2e, 0xff});

        auto& [sw, sh] = getScreenSize();

        TTF_Font* fontBig = FontManager::instance().load("assets/Roboto-Regular.ttf", 64.f);
        TTF_Font* fontSml = FontManager::instance().load("assets/Roboto-Regular.ttf", 24.f);

        auto ui = addObject("UI");
        ui->transform.position = {sw * 0.5f, sh * 0.5f};

        auto title = ui->addComponent<TextComponent>("Title", "GAME OVER", fontBig);
        title->setOffset({0.f, -60.f});

        auto hint = ui->addComponent<TextComponent>("Hint", "[ENTER] Back to Menu", fontSml);
        hint->setOffset({0.f, 60.f});

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