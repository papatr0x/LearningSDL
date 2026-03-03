// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_PLAYER_H
#define LETSLEARNSDL_PLAYER_H
#include "GameObject.h"

class Player : public SceneGameObject {
public:
    void draw(SDL_Renderer* renderer) override {
        SceneGameObject::draw(renderer);
        std::cout << "Drawing Player\n";
    }
    void update() override {
        std::cout << "Updating Player\n";
        m_x += 10;
        m_y += 20;
    }
    void clean() override {
        SceneGameObject::clean();
        std::cout << "Cleaning Player\n";

    }
};

#endif //LETSLEARNSDL_PLAYER_H