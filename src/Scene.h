// Created by Patricio Palma on 07-03-26.

#ifndef LETSLEARNSDL_SCENE_H
#define LETSLEARNSDL_SCENE_H
#include "Object.h"
#include <memory>
#include <vector>
#include "SDL3/SDL_render.h"

class Scene {
public:
    Scene() {
        componentPool.reserve(256);
    }
    virtual ~Scene() = default;

    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene&&)      = delete;

    virtual void load()   = 0;
    virtual void unload() {}

    void render(SDL_Renderer* renderer) {
        SDL_RenderClear(renderer);
        for (auto& [owner, comp] : componentPool) {
            if (owner->isActive() && comp->isEnabled())
                comp->render(renderer);
        }
        SDL_RenderPresent(renderer);
    }

    void update(const float deltaTime) {
        for (auto& [owner, comp] : componentPool) {
            if (owner->isActive() && comp->isEnabled())
                comp->update(deltaTime);
        }
    }

    template<typename T, typename ... Args>
    T* addObject(Args&&... args) {
        static_assert(std::is_base_of_v<Object, T>, "T MUST derive of Object");

        auto object = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = object.get();

        // Every component added to this object is registered in the flat pool.
        ptr->onComponentAdded = [this, owner = ptr](Component* c) {
            componentPool.push_back({owner, c});
        };

        objects.push_back(std::move(object));
        return ptr;
    }

    Object* findObject(const std::string& name) {
        const auto it = std::find_if(objects.begin(), objects.end(),
            [&name](const std::unique_ptr<Object>& go) {
                return go->getName() == name;
            });
        return it != objects.end() ? it->get() : nullptr;
    }

private:
    struct ComponentEntry { Object* owner; Component* component; };
    std::vector<ComponentEntry> componentPool;
    std::vector<std::unique_ptr<Object>> objects;
};
#endif //LETSLEARNSDL_SCENE_H