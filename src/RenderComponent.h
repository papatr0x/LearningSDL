// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_RENDERCOMPONENT_H
#define LETSLEARNSDL_RENDERCOMPONENT_H
#include "Component.h"
#include "Texture.h"
#include "Vector.h"
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

    void setPivot(const Vec2F& p) { pivot = p; }
    const Vec2F& getPivot() const { return pivot; }

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

        const float w{static_cast<float>(texture->getWidth())};
        const float h{static_cast<float>(texture->getHeight())};

        SDL_FRect dst {
            owner->transform.position.x - pivot.x * w,
            owner->transform.position.y - pivot.y * h,
            w, h
        };

        if (owner->transform.rotation != 0.0f ) {
            const SDL_FPoint center { pivot.x * w, pivot.y * h };
            SDL_RenderTextureRotated(renderer, texture->get(), nullptr, &dst,
                owner->transform.rotation, &center, SDL_FLIP_NONE);
        }
        else {
            SDL_RenderTexture(renderer, texture->get(), nullptr, &dst);
        }
    }

private:
    Texture* texture{};       // non-owning

    // Pivot is like in Unity. Axis starts at top-left.
    // 0 = start of the axis, 0.5 = middle, 1.0 = end of the axis
    Vec2F pivot{0.5f, 0.5f};
};

#endif //LETSLEARNSDL_RENDERCOMPONENT_H
