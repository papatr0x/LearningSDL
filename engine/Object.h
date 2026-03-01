// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_OBJECT_H
#define LETSLEARNSDL_OBJECT_H
#include "Component.h"
#include "Transform.h"
#include <vector>
#include <memory>
#include <functional>
#include <string_view>

class Scene;

class Object {
public:
    // By definition, no component is added on Object ctor or its derived classes.
    explicit Object(std::string name) : name(std::move(name)) { }

    // Not copyable
    Object(const Object&)            = delete;
    Object& operator=(const Object&) = delete;

    // Not movable
    Object(Object&&)            = delete;
    Object& operator=(Object&&) = delete;

    virtual ~Object() = default;

    friend class Scene;

    template<typename T, typename ... Args>
    T* addComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T MUST derive of Component");

        auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = component.get();
        components.push_back(std::move(component));
        if (onComponentAdded)
            onComponentAdded(ptr);
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
    T* getComponent(std::string_view componentName) {
        static_assert(std::is_base_of_v<Component, T>, "T MUST derive of Component");

        for (auto& c : components) {
            if (c->getName() == componentName)
                return dynamic_cast<T*>(c.get());
        }
        return nullptr;
    }

    const std::string& getName() const { return name; }
    bool isActive() const { return active; }
    void setActive(bool value) { active = value; }
    const std::string& getTag() const { return tag; }
    void setTag(const std::string& newTag) { tag = newTag; }
    bool isPendingDestroy() const { return pendingDestroy; }
    void destroy() { pendingDestroy = true; }
    Scene* getScene() const { return scene; }

    Transform transform;

private:
    Scene* scene{};
    std::function<void(Component*)> onComponentAdded;
    std::vector<std::unique_ptr<Component>> components;

    void flushDestroyedComponents() {
        components.erase(
            std::remove_if(components.begin(), components.end(),
                [](const std::unique_ptr<Component>& c) { return c->isPendingDestroy(); }),
            components.end());
    }

protected:
    std::string name;
    std::string tag;
    bool active{true};
    bool pendingDestroy{false};
};

#endif //LETSLEARNSDL_OBJECT_H
