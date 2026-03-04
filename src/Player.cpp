// Created by Patricio Palma on 03-03-26.

#include "Player.h"

// void Player::load(int x, int y, int width, int height, const TextureId& id) {
//     SceneGameObject::load(x, y, width, height, id);
// }

// void Player::draw(SDL_Renderer* renderer) {
//     SceneGameObject::draw(renderer);
//     std::cout << "Drawing Player\n";
// }

void Player::update(float deltaTime) {
    m_x += 100.0f * deltaTime;
}

void Player::clean() {
    SceneGameObject::clean();
    std::cout << "Cleaning Player\n";

}
