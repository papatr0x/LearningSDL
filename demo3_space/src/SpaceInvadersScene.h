// Created by Patricio Palma on 20-03-26.

#ifndef LETSLEARNSDL_SPACEINVADERSSCENE_H
#define LETSLEARNSDL_SPACEINVADERSSCENE_H

#include "Scene.h"

class TextComponent;

class SpaceInvadersScene : public Scene {
public:
    void load() override;
private:
    TextComponent*       playerScoreTextCompo_{};
    std::vector<Object*> martians_{};
};

#endif //LETSLEARNSDL_SPACEINVADERSSCENE_H