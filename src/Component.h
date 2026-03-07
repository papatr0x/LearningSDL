// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_COMPONENT_H
#define LETSLEARNSDL_COMPONENT_H
#include <string>

struct SDL_Renderer;
class Object;

class Component {
public:
    explicit Component(Object* owner, std::string name) : owner(owner), name(std::move(name)) {}
    virtual ~Component() = default;

    // Not copyable or movable — components belong to a single Object.
    Component(const Component&)            = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&)                 = delete;
    Component& operator=(Component&&)      = delete;

    virtual void onStart() noexcept {}
    virtual void updateComponent(float dt) noexcept {}
    virtual void updatePhysics(float fixedDt) noexcept {}
    virtual void render(SDL_Renderer* renderer) noexcept {}

    bool isEnabled() const { return enabled; }
    void setEnabled(bool value) { enabled = value; }
    const std::string& getName() const { return name; }
    void setUpdateInterval(float seconds) { intervalSeconds = seconds; }
    float getUpdateInterval() const { return intervalSeconds; }
    bool isPendingDestroy() const { return pendingDestroy; }
    void destroy() { pendingDestroy = true; }
    int getRenderLayer() const { return renderLayer; }
    void setRenderLayer(const int order) { renderLayer = order; }

protected:
    std::string name;
    bool enabled{true};
    float intervalSeconds{0.0f};
    bool pendingDestroy{false};
    int renderLayer{0};
    Object* owner{};
};

#endif //LETSLEARNSDL_COMPONENT_H
