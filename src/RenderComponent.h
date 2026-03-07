// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_RENDERCOMPONENT_H
#define LETSLEARNSDL_RENDERCOMPONENT_H
#include "Component.h"
#include "Texture.h"
#include <SDL3/SDL.h>

class RenderComponent : public Component {
public:
    explicit RenderComponent(Object* owner, const std::string& name) : Component(owner, name) {}

    void setTexture(Texture* tex) {
        if (!tex || !tex->isValid()) {
            SDL_Log("RenderComponent::setTexture - texture is nullptr or invalid");
            return;
        }
        texture = tex;
    }

    void render(SDL_Renderer* renderer) noexcept override {
        if (!isEnabled())
            return;
        if (!renderer) {
            SDL_Log("RenderComponent::render - renderer is nullptr");
            return;
        }
        if (!texture || !texture->isValid()) {
            SDL_Log("RenderComponent::render - invalid texture");
            return;
        }

        SDL_FRect dst { owner->transform.position.x, owner->transform.position.y,
            (float)texture->getWidth(), (float)texture->getHeight() };

        if (owner->transform.rotation != 0.0f) {
            SDL_RenderTextureRotated(
                    renderer, texture->get(), nullptr, &dst, owner->transform.rotation,
                    nullptr, SDL_FLIP_NONE);
        } else {
            SDL_RenderTexture(renderer, texture->get(), nullptr, &dst);
        }
    }

private:
    Texture* texture{};    // no-owning
};

#endif //LETSLEARNSDL_RENDERCOMPONENT_H
