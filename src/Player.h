// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_PLAYER_H
#define LETSLEARNSDL_PLAYER_H

#include "Object.h"

class Player : public Object {
public:
    explicit Player(const std::string& name) : Object(name) { }
    // void load(int x, int y, int width, int height, const TextureId& id) override;
    // void draw(SDL_Renderer* renderer) override;
    // void update(float deltaTime) override;
    // void clean() override;
};

#endif //LETSLEARNSDL_PLAYER_H