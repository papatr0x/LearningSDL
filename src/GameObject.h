// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_GAMEOBJECT_H
#define LETSLEARNSDL_GAMEOBJECT_H
#include <SDL3/SDL_render.h>
#include <iostream>

class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void update(float deltaTime) { (void)deltaTime; std::cout << "Updating GameObject\n"; }
};

class SceneGameObject : public GameObject {
public:
    virtual void draw(SDL_Renderer* renderer);

    virtual void clean();

    virtual void load(float x, float y, float width, float height, const std::string& textureId);
    void update(float deltaTime) override;

protected:
    float m_x{};
    float m_y{};

    // this should be migrated to a texture separate class
    std::string m_textureId;
    int m_currentFrame{};
    int m_currentRow{};
    float m_width{};
    float m_height{};
};

#endif //LETSLEARNSDL_GAMEOBJECT_H