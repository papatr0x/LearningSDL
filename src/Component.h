// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_COMPONENT_H
#define LETSLEARNSDL_COMPONENT_H
#include <string>

struct SDL_Renderer;

class Component {
public:
    explicit Component(std::string name) : name(std::move(name)) { }
    virtual ~Component() = default;

    // No copy nor move. Components belong to a single GameObject
    Component(const Component&)            = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&)                 = delete;
    Component& operator=(Component&&)      = delete;

    virtual void update(float dt) {}
    virtual void render(SDL_Renderer* renderer) {}

    bool isEnabled() const { return enabled; }
    void setEnabled(bool value) { enabled = value; }
    std::string getName() const { return name; }

private:
    std::string name;
    bool enabled = true;
};

#endif //LETSLEARNSDL_COMPONENT_H