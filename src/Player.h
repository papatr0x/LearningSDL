// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_PLAYER_H
#define LETSLEARNSDL_PLAYER_H

#include "GameObject.h"

using TextureId = std::string;

class Player : public SceneGameObject {
public:
    // void load(int x, int y, int width, int height, const TextureId& id) override;
    // void draw(SDL_Renderer* renderer) override;
    void update(float deltaTime) override;
    void clean() override;
};

#endif //LETSLEARNSDL_PLAYER_H