// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_COLLIDERCOMPONENT_H
#define LETSLEARNSDL_COLLIDERCOMPONENT_H

#include "Component.h"
#include "Object.h"
#include "Scene.h"
#include "Vector.h"
#include <SDL3/SDL_render.h>
#include <functional>
#include <algorithm>
#include <cmath>
#include <numbers>

class BoxColliderComponent;
class CircleColliderComponent;

class ColliderComponent : public Component {
public:
    using CollisionCallback = std::function<void(ColliderComponent* other)>;

    ColliderComponent(Object* owner, const std::string& id, const Vec2F offset = {0.f, 0.f})
        : Component(owner, id), offset(offset) {}

    ~ColliderComponent() override = default;

    // Returns the AABB wrapping this collider (used for broad phase).
    virtual SDL_FRect getBounds() const = 0;

    Vec2F getCenter() const {
        return { owner->transform.position.x + offset.x,
                 owner->transform.position.y + offset.y };
    }

    // Precise narrow-phase overlap test.
    bool overlaps(const ColliderComponent& other) const { return testAgainst(other); }

    void render(SDL_Renderer* renderer) noexcept override {
        if (!debugDraw) return;
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200);
        drawCollider(renderer);
    }

    virtual void drawCollider(SDL_Renderer* renderer) noexcept = 0;

    // Public to allow cross-subclass access required by double dispatch.
    virtual bool testAgainst(const ColliderComponent& other) const = 0;
    virtual bool testAgainstBox(const BoxColliderComponent& box) const = 0;
    virtual bool testAgainstCircle(const CircleColliderComponent& circle) const = 0;

    bool isTrigger{false};
    bool debugDraw{false};

    CollisionCallback onEnter;
    CollisionCallback onStay;
    CollisionCallback onExit;

protected:
    Vec2F offset{};

    static bool test(const BoxColliderComponent& a, const BoxColliderComponent& b);
    static bool test(const CircleColliderComponent& a, const CircleColliderComponent& b);
    static bool test(const BoxColliderComponent& box, const CircleColliderComponent& circle);
};

class BoxColliderComponent : public ColliderComponent {
public:
    BoxColliderComponent(Object* owner, const std::string& id, const Vec2F size, const Vec2F offset = {0.f, 0.f})
        : ColliderComponent(owner, id, offset), size(size) {}

    void drawCollider(SDL_Renderer* renderer) noexcept override {
        SDL_FRect bounds = getBounds();
        const Vec2F camOff = screenSpace ? Vec2F{} : owner->getScene()->getWorldRenderOffset();
        bounds.x += camOff.x;
        bounds.y += camOff.y;
        SDL_RenderRect(renderer, &bounds);
    }

    SDL_FRect getBounds() const override {
        const Vec2F center = getCenter();
        return { center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x, size.y };
    }

    Vec2F getSize() const { return size; }

    bool testAgainst(const ColliderComponent& other) const override {
        return other.testAgainstBox(*this);
    }
    bool testAgainstBox(const BoxColliderComponent& box) const override {
        return test(box, *this);
    }
    bool testAgainstCircle(const CircleColliderComponent& circle) const override {
        return test(*this, circle);
    }

private:
    Vec2F size{};
};

class CircleColliderComponent : public ColliderComponent {
public:
    CircleColliderComponent(Object* owner, const std::string& id, const float radius, const Vec2F offset = {0.f, 0.f})
        : ColliderComponent(owner, id, offset), radius(radius) {}

    void drawCollider(SDL_Renderer* renderer) noexcept override {
        constexpr int segments = 24;
        const Vec2F camOff = screenSpace ? Vec2F{} : owner->getScene()->getWorldRenderOffset();
        const Vec2F raw = getCenter();
        const Vec2F center { raw.x + camOff.x, raw.y + camOff.y };
        const float step = 2.f * std::numbers::pi_v<float> / segments;
        for (int i = 0; i < segments; i++) {
            const float a1 = step * i;
            const float a2 = step * (i + 1);
            SDL_RenderLine(renderer,
                center.x + std::cos(a1) * radius, center.y + std::sin(a1) * radius,
                center.x + std::cos(a2) * radius, center.y + std::sin(a2) * radius);
        }
    }

    SDL_FRect getBounds() const override {
        const Vec2F center = getCenter();
        return { center.x - radius, center.y - radius, radius * 2.f, radius * 2.f };
    }

    float getRadius() const { return radius; }

    bool testAgainst(const ColliderComponent& other) const override {
        return other.testAgainstCircle(*this);
    }
    bool testAgainstBox(const BoxColliderComponent& box) const override {
        return test(box, *this);
    }
    bool testAgainstCircle(const CircleColliderComponent& circle) const override {
        return test(*this, circle);
    }

private:
    float radius{0.f};
};

inline bool ColliderComponent::test(const BoxColliderComponent& a, const BoxColliderComponent& b) {
    const SDL_FRect ra = a.getBounds();
    const SDL_FRect rb = b.getBounds();
    return ra.x < rb.x + rb.w && ra.x + ra.w > rb.x &&
           ra.y < rb.y + rb.h && ra.y + ra.h > rb.y;
}

inline bool ColliderComponent::test(const CircleColliderComponent& a, const CircleColliderComponent& b) {
    const Vec2F ca = a.getCenter();
    const Vec2F cb = b.getCenter();
    const float dx = ca.x - cb.x;
    const float dy = ca.y - cb.y;
    const float ra = a.getRadius();
    const float rb = b.getRadius();
    const float rSum = ra + rb;
    return dx * dx + dy * dy < rSum * rSum;
}

inline bool ColliderComponent::test(const BoxColliderComponent& box, const CircleColliderComponent& circle) {
    const SDL_FRect b = box.getBounds();
    const Vec2F c = circle.getCenter();
    const float closestX = std::clamp(c.x, b.x, b.x + b.w);
    const float closestY = std::clamp(c.y, b.y, b.y + b.h);
    const float dx = c.x - closestX;
    const float dy = c.y - closestY;
    const float r = circle.getRadius();
    return dx * dx + dy * dy < r * r;
}

#endif //LETSLEARNSDL_COLLIDERCOMPONENT_H
