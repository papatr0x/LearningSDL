// Created by Patricio Palma on 04-03-26.

#ifndef LETSLEARNSDL_ENEMY_H
#define LETSLEARNSDL_ENEMY_H
#include "Object.h"

class Enemy : public Object {
public:
    explicit Enemy(const std::string& name) : Object(name) { }
    // // void load(int x, int y, int width, int height, std::string
    // // textureID);
    // // void draw(SDL_Renderer* pRenderer);
    // void update(float deltaTime) override;
    // void clean() override;
};

#endif //LETSLEARNSDL_ENEMY_H
