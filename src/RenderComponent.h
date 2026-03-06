// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_RENDERCOMPONENT_H
#define LETSLEARNSDL_RENDERCOMPONENT_H
#include <SDL3/SDL.h>

#include "Component.h"
#include "Texture.h"
#include "Transform.h"

class RenderComponent : public Component {
public:
    explicit RenderComponent(const std::string& name) : Component(name) { }

    void setTexture(Texture* tex) {
        if (!tex) {
            SDL_Log("RenderComponent::setTexture - texture is nullptr");
            return;
        }
        if (!tex->isValid()) {
            SDL_Log("RenderComponent::setTexture - texture is not valid");
            return;
        }
        texture = tex;
    }

    // Non-owning Transform: RenderComponent reads position and size from it.
    void setTransformPtr(const Transform* t) { transform = t; }

    void render(SDL_Renderer* renderer) noexcept override {
        if (!isEnabled()) return;
        if (!renderer) {
            SDL_Log("RenderComponent::render - renderer is nullptr");
            return;
        }
        if (!texture || !texture->isValid()) {
            SDL_Log("RenderComponent::render - invalid texture");
            return;
        }
        if (!transform) {
            SDL_Log("RenderComponent::render - no Transform set, nothing to render");
            return;
        }

        // Base size: natural texture size, overridden by Transform if set
        float w = texture->getWidth();
        float h = texture->getHeight();
        float x = 0.0f;
        float y = 0.0f;

        if (transform) {
            x = transform->x;
            y = transform->y;
            if (transform->width  > 0.0f) w = transform->width;
            if (transform->height > 0.0f) h = transform->height;
            w *= transform->scaleX;
            h *= transform->scaleY;
        }

        SDL_FRect dst { x, y, w, h };

        if (transform && transform->rotation != 0.0f) {
            SDL_RenderTextureRotated(renderer, texture->get(), nullptr, &dst,
                                     transform->rotation, nullptr, SDL_FLIP_NONE);
        } else {
            SDL_RenderTexture(renderer, texture->get(), nullptr, &dst);
        }
    }

private:
    Texture*    texture{};    // no-owning
    const Transform*  transform{};  // no-owning
};

#endif //LETSLEARNSDL_RENDERCOMPONENT_H
