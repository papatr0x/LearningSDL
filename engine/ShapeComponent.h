// Created by Patricio Palma on 20-03-26.

#ifndef LETSLEARNSDL_SHAPECOMPONENT_H
#define LETSLEARNSDL_SHAPECOMPONENT_H

#include "Component.h"
#include "Object.h"
#include "Scene.h"
#include "Vector.h"
#include <SDL3/SDL_render.h>
#include <cmath>
#include <numbers>
#include <vector>

// ---------------------------------------------------------------------------
// ShapeComponent — abstract base for all primitive shape components.
//
// Shapes are centered on transform.position + offset.
// Respects screenSpace and camera offset (same contract as RenderComponent).
// Subclasses implement drawShape(renderer, worldCenter).
// ---------------------------------------------------------------------------
class ShapeComponent : public Component {
public:
    ShapeComponent(Object* owner, const std::string& id, SDL_Color color, bool filled = true)
        : Component(owner, id), color_(color), filled_(filled) {}

    void      setColor(SDL_Color color) { color_  = color;  }
    SDL_Color getColor()          const { return color_;    }
    void      setFilled(bool filled)    { filled_ = filled; }
    bool      isFilled()          const { return filled_;   }
    void      setOffset(Vec2F offset)   { offset_ = offset; }
    Vec2F     getOffset()         const { return offset_;   }

    // Final: subclasses customise via drawShape(), not render().
    void render(SDL_Renderer* renderer) noexcept override final {
        if (!renderer) return;
        SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);
        const Vec2F camOff = screenSpace ? Vec2F{} : owner->getScene()->getWorldRenderOffset();
        const Vec2F center {
            owner->transform.position.x + offset_.x + camOff.x,
            owner->transform.position.y + offset_.y + camOff.y
        };
        drawShape(renderer, center);
    }

protected:
    virtual void drawShape(SDL_Renderer* renderer, Vec2F center) noexcept = 0;

    SDL_Color color_{255, 255, 255, 255};
    bool      filled_{true};
    Vec2F     offset_{};
};

// ---------------------------------------------------------------------------
// SquareComponent — axis-aligned rectangle.
// Uses fast SDL_RenderFillRect / SDL_RenderRect when unrotated.
// Falls back to SDL_RenderGeometry when transform.rotation != 0.
// ---------------------------------------------------------------------------
class SquareComponent : public ShapeComponent {
public:
    SquareComponent(Object* owner, const std::string& id, Vec2F size, SDL_Color color, bool filled = true)
        : ShapeComponent(owner, id, color, filled), size_(size) {}

    void  setSize(Vec2F size) { size_ = size; }
    Vec2F getSize()     const { return size_; }

protected:
    void drawShape(SDL_Renderer* renderer, Vec2F center) noexcept override {
        const float rot = owner->transform.rotation;

        // Fast path — no rotation
        if (rot == 0.f) {
            const SDL_FRect rect {
                center.x - size_.x * 0.5f,
                center.y - size_.y * 0.5f,
                size_.x, size_.y
            };
            filled_ ? SDL_RenderFillRect(renderer, &rect)
                    : SDL_RenderRect(renderer, &rect);
            return;
        }

        // Rotated path — 4 corners transformed and drawn via SDL_RenderGeometry
        const float rad  = rot * std::numbers::pi_v<float> / 180.f;
        const float cosA = std::cos(rad);
        const float sinA = std::sin(rad);
        const float hw   = size_.x * 0.5f;
        const float hh   = size_.y * 0.5f;

        const SDL_FColor fc { color_.r / 255.f, color_.g / 255.f,
                              color_.b / 255.f, color_.a / 255.f };

        // Corners in local space: TL, TR, BR, BL
        const float lx[4] = { -hw,  hw,  hw, -hw };
        const float ly[4] = { -hh, -hh,  hh,  hh };

        SDL_Vertex verts[4];
        for (int i = 0; i < 4; ++i) {
            verts[i].position = {
                center.x + lx[i] * cosA - ly[i] * sinA,
                center.y + lx[i] * sinA + ly[i] * cosA
            };
            verts[i].color = fc;
        }

        if (filled_) {
            constexpr int idx[] = { 0, 1, 2, 0, 2, 3 };
            SDL_RenderGeometry(renderer, nullptr, verts, 4, idx, 6);
        } else {
            for (int i = 0; i < 4; ++i) {
                const int j = (i + 1) % 4;
                SDL_RenderLine(renderer,
                    verts[i].position.x, verts[i].position.y,
                    verts[j].position.x, verts[j].position.y);
            }
        }
    }

private:
    Vec2F size_;
};

// ---------------------------------------------------------------------------
// CircleComponent — smooth circle approximation (32 segments).
// Vertex and index buffers are pre-allocated at construction so drawShape()
// performs no heap allocation per frame. Respects transform.rotation.
// ---------------------------------------------------------------------------
class CircleComponent : public ShapeComponent {
public:
    CircleComponent(Object* owner, const std::string& id,
                    float radius, SDL_Color color, bool filled = true)
        : ShapeComponent(owner, id, color, filled), radius_(radius)
    {
        buildBuffers();
    }

    void  setRadius(float r) { radius_ = r; }
    float getRadius()  const { return radius_; }

protected:
    void drawShape(SDL_Renderer* renderer, Vec2F center) noexcept override {
        const float rotRad = owner->transform.rotation * std::numbers::pi_v<float> / 180.f;
        const float step   = 2.f * std::numbers::pi_v<float> / static_cast<float>(segments_);
        const SDL_FColor fc { color_.r / 255.f, color_.g / 255.f,
                              color_.b / 255.f, color_.a / 255.f };

        if (filled_) {
            verts_[0].position = { center.x, center.y };
            verts_[0].color    = fc;
            for (int i = 0; i <= segments_; ++i) {
                const float angle    = step * static_cast<float>(i) + rotRad;
                verts_[i + 1].position = {
                    center.x + std::cos(angle) * radius_,
                    center.y + std::sin(angle) * radius_
                };
                verts_[i + 1].color = fc;
            }
            SDL_RenderGeometry(renderer, nullptr,
                verts_.data(),   static_cast<int>(verts_.size()),
                indices_.data(), static_cast<int>(indices_.size()));
        } else {
            for (int i = 0; i < segments_; ++i) {
                const float a1 = step * static_cast<float>(i)     + rotRad;
                const float a2 = step * static_cast<float>(i + 1) + rotRad;
                SDL_RenderLine(renderer,
                    center.x + std::cos(a1) * radius_,
                    center.y + std::sin(a1) * radius_,
                    center.x + std::cos(a2) * radius_,
                    center.y + std::sin(a2) * radius_);
            }
        }
    }

private:
    void buildBuffers() {
        verts_.resize(segments_ + 2);   // center + N edge + closing vertex
        indices_.resize(segments_ * 3); // N triangles × 3 indices
        for (int i = 0; i < segments_; ++i) {
            indices_[i * 3 + 0] = 0;
            indices_[i * 3 + 1] = i + 1;
            indices_[i * 3 + 2] = i + 2;
        }
    }

    static constexpr int    segments_{32};
    float                   radius_;
    std::vector<SDL_Vertex> verts_;
    std::vector<int>        indices_;
};

#endif //LETSLEARNSDL_SHAPECOMPONENT_H
