// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_INTERMEDIATESCENE_H
#define LETSLEARNSDL_INTERMEDIATESCENE_H

#include "Scene.h"
#include "GameEngine.h"
#include "FontManager.h"
#include "TextComponent.h"
#include "TimerComponent.h"
#include "TransitionComponent.h"

class IntermediateScene : public Scene {
public:
    void load() override {
        int screenWidth, screenHeight;
        SDL_GetWindowSize(GameEngine::instance().getWindow(), &screenWidth, &screenHeight);
        TTF_Font* font = FontManager::instance().load("assets/Roboto-Regular.ttf", 24.f);

        auto text = addObject<Object>("TransitionText");
        text->transform.position = {screenWidth / 2.f, screenHeight / 2.f};
        text->addComponent<TextComponent>("Label", "Loading...", font);

        auto timerObject = addObject<Object>("Timer");
        auto tc = timerObject->addComponent<TransitionComponent>("Complete"); // empty = completePendingTransition
        timerObject->addComponent<TimerComponent>("Timer", 2.f, [tc]{ tc->trigger(); });
    }
};

#endif //LETSLEARNSDL_INTERMEDIATESCENE_H
