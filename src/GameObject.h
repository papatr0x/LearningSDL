// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_GAMEOBJECT_H
#define LETSLEARNSDL_GAMEOBJECT_H
#include <SDL3/SDL_render.h>
#include <iostream>

class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void update() { std::cout << "Updating GameObject\n"; }
};

class SceneGameObject : public GameObject {
public:
    virtual void draw(SDL_Renderer* renderer) { std::cout << "Drawing SceneGameObject\n"; }
    virtual void clean() { std::cout << "Cleaning SceneGameObject\n"; }
    void load(int x, int y, int width, int height, const std::string& textureId);
protected:
    int m_x{};
    int m_y{};
    //int m_scale{}
    //int m_rotation{};

    // this should be migrated to a texture separate class
    std::string m_textureId;
    int m_currentFrame{};
    int m_currentRow{};
    int m_width{};
    int m_height{};
};

#endif //LETSLEARNSDL_GAMEOBJECT_H