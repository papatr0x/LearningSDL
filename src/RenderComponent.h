// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_RENDERCOMPONENT_H
#define LETSLEARNSDL_RENDERCOMPONENT_H
#include <SDL3/SDL.h>

#include "Component.h"
#include "Texture.h"

class RenderComponent : public Component {
public:
    RenderComponent() = default;

    void setTexture(Texture* tex) {
        if (!tex) {
            SDL_Log("RenderComponent::setTexture - texture es nullptr");
            return;
        }
        if (!tex->isValid()) {
            SDL_Log("RenderComponent::setTexture - texture no es valida");
            return;
        }
        texture = tex;

        // Ajusta dst al tamaño natural de la textura por defecto
        dst.w = static_cast<float>(tex->getWidth());
        dst.h = static_cast<float>(tex->getHeight());
    }

    void setDestination(float x, float y, float w, float h) {
        dst = { x, y, w, h };
    }

    void render(SDL_Renderer* renderer) override {
        if (!isEnabled()) return;
        if (!renderer) {
            SDL_Log("RenderComponent::render - renderer es nullptr");
            return;
        }
        if (!texture || !texture->isValid()) {
            SDL_Log("RenderComponent::render - textura no valida");
            return;
        }

        SDL_RenderTexture(renderer, texture->get(), nullptr, &dst);
    }

private:
    Texture* texture = nullptr;  // no owning, solo referencia
    SDL_FRect dst = {};
};

#endif //LETSLEARNSDL_RENDERCOMPONENT_H