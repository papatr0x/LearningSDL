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

    virtual void update(float dt) noexcept {}
    virtual void render(SDL_Renderer* renderer) noexcept {}

    bool isEnabled() const { return enabled; }
    void setEnabled(bool value) { enabled = value; }
    const std::string& getName() const { return name; }

protected:
    std::string name;
    bool enabled = true;
    Object* owner{};
};

#endif //LETSLEARNSDL_COMPONENT_H
