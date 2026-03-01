// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_RENDERCOMPONENT_H
#define LETSLEARNSDL_RENDERCOMPONENT_H
#include "Component.h"
#include "Scene.h"
#include "Texture.h"
#include "Vector.h"
#include <SDL3/SDL.h>
#include <optional>

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

    void setSourceRect(const SDL_FRect& rect) { sourceRect = rect; }
    void clearSourceRect() { sourceRect.reset(); }

    void render(SDL_Renderer* renderer) noexcept override {
        if (!renderer) {
            SDL_Log("RenderComponent::render - renderer is nullptr");
            return;
        }
        if (!texture || !texture->isValid()) {
            SDL_Log("RenderComponent::render - invalid texture");
            return;
        }

        const SDL_FRect* src = sourceRect ? &*sourceRect : nullptr;
        const float w{src ? src->w : static_cast<float>(texture->getWidth())};
        const float h{src ? src->h : static_cast<float>(texture->getHeight())};

        const Vec2F camOff = screenSpace ? Vec2F{} : owner->getScene()->getWorldRenderOffset();
        SDL_FRect dst {
            owner->transform.position.x + offset.x - pivot.x * w + camOff.x,
            owner->transform.position.y + offset.y - pivot.y * h + camOff.y,
            w, h
        };

        if (owner->transform.rotation != 0.0f) {
            const SDL_FPoint center { pivot.x * w, pivot.y * h };
            SDL_RenderTextureRotated(renderer, texture->get(), src, &dst,
                owner->transform.rotation, &center, SDL_FLIP_NONE);
        }
        else {
            SDL_RenderTexture(renderer, texture->get(), src, &dst);
        }
    }

private:
    Texture* texture{};                    // non-owning
    std::optional<SDL_FRect> sourceRect{}; // when set, only this region of the texture is drawn

    // Pivot is like in Unity. Axis starts at top-left.
    // 0 = start of the axis, 0.5 = middle, 1.0 = end of the axis
    Vec2F pivot{0.5f, 0.5f};
    Vec2F offset{};
};

#endif //LETSLEARNSDL_RENDERCOMPONENT_H
