// Created by Patricio Palma on 07-03-26.

#ifndef LETSLEARNSDL_SCENE_H
#define LETSLEARNSDL_SCENE_H
#include "Object.h"
#include <memory>
#include <vector>
#include <algorithm>
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

    virtual void load() = 0;
    virtual void unload() {}

    void start() {
        load();
        for (auto& entry : componentPool)
            entry.component->onStart();
    }

    void render(SDL_Renderer* renderer) {
        SDL_RenderClear(renderer);
        for (auto& entry : componentPool) {
            if (entry.owner->isActive() && entry.component->isEnabled())
                entry.component->render(renderer);
        }
        SDL_RenderPresent(renderer);
    }

    void update(const float deltaTime) {
        for (auto& entry : componentPool) {
            if (!entry.owner->isActive() || !entry.component->isEnabled()) continue;

            const float interval = entry.component->getUpdateInterval();
            if (interval <= 0.0f) {
                entry.component->updateComponent(deltaTime);
            } else {
                entry.intervalAccumulator += deltaTime;
                if (entry.intervalAccumulator >= interval) {
                    const float accumulatedDt = entry.intervalAccumulator;
                    entry.intervalAccumulator = 0.0f;
                    entry.component->updateComponent(accumulatedDt);
                }
            }
        }

        physicsAccumulator += deltaTime;
        while (physicsAccumulator >= fixedPhysicsStep) {
            for (auto& entry : componentPool) {
                if (entry.owner->isActive() && entry.component->isEnabled())
                    entry.component->updatePhysics(fixedPhysicsStep);
            }
            physicsAccumulator -= fixedPhysicsStep;
        }

        flushDestroyQueue();
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
    void flushDestroyQueue() {
        componentPool.erase(
            std::remove_if(componentPool.begin(), componentPool.end(),
                [](const ComponentEntry& e) { return e.owner->isPendingDestroy(); }),
            componentPool.end());

        objects.erase(
            std::remove_if(objects.begin(), objects.end(),
                [](const std::unique_ptr<Object>& o) { return o->isPendingDestroy(); }),
            objects.end());
    }

    struct ComponentEntry { Object* owner; Component* component; float intervalAccumulator = 0.0f; };
    std::vector<ComponentEntry> componentPool;
    std::vector<std::unique_ptr<Object>> objects;
    float physicsAccumulator = 0.0f;
    static constexpr float fixedPhysicsStep = 1.0f / 60.0f;
};
#endif //LETSLEARNSDL_SCENE_H