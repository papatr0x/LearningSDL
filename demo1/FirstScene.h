// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_FIRSTSCENE_H
#define LETSLEARNSDL_FIRSTSCENE_H

#include "Scene.h"
#include "GameEngine.h"
#include "FontManager.h"
#include "TextComponent.h"
#include "TimerComponent.h"
#include "TransitionComponent.h"

class FirstScene : public Scene {
private:
    static constexpr float delayTime = 1.f;
public:
    void load() override {
        int screenWidth, screenHeight;
        SDL_GetWindowSize(GameEngine::instance().getWindow(), &screenWidth, &screenHeight);
        TTF_Font* font = FontManager::instance().load("assets/Roboto-Bold.ttf", 32.f);

        auto logo = addObject("Logo");
        logo->transform.position = {screenWidth / 2.f, screenHeight / 2.f};
        logo->addComponent<TextComponent>("Label", "MY GAME STUDIO", font);

        auto timer = addObject("Timer");
        auto tc = timer->addComponent<TransitionComponent>("ToMenu", "menu");
        timer->addComponent<TimerComponent>("Timer", delayTime, [tc]{ tc->trigger(); });
    }
};

#endif //LETSLEARNSDL_FIRSTSCENE_H
