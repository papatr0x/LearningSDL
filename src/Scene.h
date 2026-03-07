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
        started = true;
        for (auto& entry : componentPool)
            entry.component->onStart();
    }

    void render(SDL_Renderer* renderer) {
        SDL_RenderClear(renderer);

        renderables.clear();
        for (auto& entry : componentPool) {
            if (entry.owner->isActive() && entry.component->isEnabled())
                renderables.push_back(entry.component);
        }
        std::sort(renderables.begin(), renderables.end(), [](const Component* a, const Component* b) {
            return a->getRenderLayer() < b->getRenderLayer();
        });
        for (auto* component : renderables)
            component->render(renderer);

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
            if (started)
                c->onStart();
        };

        objects.push_back(std::move(object));
        return ptr;
    }

    Object* findObject(std::string_view name) {
        const auto it = std::find_if(objects.begin(), objects.end(),
            [name](const std::unique_ptr<Object>& go) {
                return go->getName() == name;
            });
        return it != objects.end() ? it->get() : nullptr;
    }

private:
    void flushDestroyQueue() {
        std::erase_if(componentPool,
                      [](const ComponentEntry& e) {
                          return e.owner->isPendingDestroy() || e.component->isPendingDestroy();
                      });

        for (auto& object : objects)
            object->flushDestroyedComponents();

        std::erase_if(objects,
                      [](const std::unique_ptr<Object>& o) { return o->isPendingDestroy(); });
    }

    // This flag allows the onStart method to be called after the component is registered, even if it is
    // created outside of the load() method.
    bool started{false};

    // These are used to calculate when to call the fixed step update (to be used for physics).
    float physicsAccumulator{0.0f};
    static constexpr float fixedPhysicsStep{1.0f / 60.0f};

    // These holds Objects in Scene while providing better locality.
    struct ComponentEntry { Object* owner; Component* component; float intervalAccumulator{0.0f}; };
    std::vector<ComponentEntry> componentPool;
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<Component*> renderables;
};
#endif //LETSLEARNSDL_SCENE_H