// Created by Patricio Palma on 03-03-26.

#include "GameObject.h"
#include "TextureManager.h"

void SceneGameObject::draw(SDL_Renderer* renderer) {
    TextureManager::instance().drawFrame(m_textureId, m_x, m_y, m_width, m_height, m_currentRow,
        m_currentFrame, renderer);
}

void SceneGameObject::clean() {
    std::cout << "Cleaning SceneGameObject\n";
}

void SceneGameObject::load(float x, float y, float width, float height, const std::string& textureId) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_textureId = textureId;
    m_currentRow = m_currentFrame = 1;
}

void SceneGameObject::update(float deltaTime) {
    m_x += 100.0f * deltaTime;
}
