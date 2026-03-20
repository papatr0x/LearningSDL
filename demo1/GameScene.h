// Created by Patricio Palma on 07-03-26.

#ifndef LETSLEARNSDL_GAMESCENE_H
#define LETSLEARNSDL_GAMESCENE_H

#include "Scene.h"
#include "AudioManager.h"

class GameScene : public Scene {
public:
    void load() override;
    void unload() override { AudioManager::instance().stopMusic(); }
};

#endif //LETSLEARNSDL_GAMESCENE_H