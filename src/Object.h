// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_OBJECT_H
#define LETSLEARNSDL_OBJECT_H
#include "Component.h"
#include <vector>
#include <memory>

class Component;

class Object {
public:
    explicit Object(std::string name) : name(std::move(name)) { }

    // No copyable
    Object(const Object&)            = delete;
    Object& operator=(const Object&) = delete;

    // Movible
    Object(Object&&)            = default;
    Object& operator=(Object&&) = default;

    ~Object() = default;

    template<typename T, typename ... Args>
    T* addComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T MUST derive of Component");

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        components.push_back(std::move(component));
        return ptr;
    }

    template<typename T>
    T* getComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T MUST derive of Component");

        for (auto& c : components) {
            if (auto* ptr = dynamic_cast<T*>(c.get()))
                return ptr;
        }
        return nullptr;
    }

    template<typename T>
    T* getComponent(const std::string& componentName) {
        static_assert(std::is_base_of_v<Component, T>, "T MUST derive of Component");

        for (auto& c : components) {
            if (c->getName() == componentName)
                return static_cast<T*>(c.get());
        }
        return nullptr;
    }

    void update(float dt) {
        if (!active) return;
        for (auto& c : components) {
            if (c->isEnabled())
                c->update(dt);
        }
    }

    void render(SDL_Renderer* renderer) {
        if (!active) return;
        if (!renderer) {
            throw std::runtime_error("Object::render - renderer is nullptr");
        }
        for (auto& c : components) {
            if (c->isEnabled())
                c->render(renderer);
        }
    }

    const std::string& getName()   const { return name; }
    bool isActive()  const { return active; }
    void setActive(bool value) { active = value; }
    const std::string& getTag() const { return tag; }
    void setTag(const std::string& newTag) { tag = newTag; }

protected:
    std::string name;
    std::string tag;
    bool active = true;
    std::vector<std::unique_ptr<Component>> components;
};

#endif //LETSLEARNSDL_OBJECT_H
