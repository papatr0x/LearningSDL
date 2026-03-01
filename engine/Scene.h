// Created by Patricio Palma on 07-03-26.

#ifndef LETSLEARNSDL_SCENE_H
#define LETSLEARNSDL_SCENE_H

#include "Object.h"
#include "SceneSubsystem.h"
#include "Camera.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <string>
#include "SDL3/SDL_render.h"

struct TTF_TextEngine;
class SceneOrchestrator;

class Scene {
public:
    Scene();
    virtual ~Scene() = default;

    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene&&)      = delete;

    virtual void load() = 0;
    virtual void unload() {}

    const std::string& getSceneId() const { return sceneId; }
    TTF_TextEngine* getTextEngine() const { return textEngine; }
    Camera& getCamera() { return camera; }
    const Vec2F& getWorldRenderOffset() const { return worldRenderOffset; }
    Vec2F screenToWorld(const Vec2F& screen) const {
        return { screen.x / camera.zoom - worldRenderOffset.x,
                 screen.y / camera.zoom - worldRenderOffset.y };
    }

    void start() {
        load();
        started = true;
        for (auto& entry : componentPool)
            entry.component->onStart();
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_RenderClear(renderer);

        renderables.clear();
        for (auto& entry : componentPool) {
            if (entry.owner->isActive() && entry.component->isEnabled())
                renderables.push_back(entry.component);
        }
        std::sort(renderables.begin(), renderables.end(), [](const Component* a, const Component* b) {
            return a->getRenderLayer() < b->getRenderLayer();
        });

        int screenW, screenH;
        SDL_GetRenderOutputSize(renderer, &screenW, &screenH);
        worldRenderOffset = {
            screenW / (2.0f * camera.zoom) - camera.position.x,
            screenH / (2.0f * camera.zoom) - camera.position.y
        };

        bool worldScaleActive = false;
        for (auto* component : renderables) {
            const bool needsWorldScale = !component->isScreenSpace();
            if (needsWorldScale != worldScaleActive) {
                SDL_SetRenderScale(renderer,
                    needsWorldScale ? camera.zoom : 1.0f,
                    needsWorldScale ? camera.zoom : 1.0f);
                worldScaleActive = needsWorldScale;
            }
            component->render(renderer);
        }
        if (worldScaleActive)
            SDL_SetRenderScale(renderer, 1.0f, 1.0f);

        SDL_RenderPresent(renderer);
    }

    void update(const float deltaTime) {
        for (auto& entry : componentPool) {
            if (!entry.owner->isActive() || !entry.component->isEnabled()) continue;

            const float interval = entry.component->getUpdateInterval();
            if (interval <= 0.0f) {
                entry.component->updateComponent(deltaTime);
            } else {
                entry.component->intervalAccumulator += deltaTime;
                if (entry.component->intervalAccumulator >= interval) {
                    const float accumulatedDt = entry.component->intervalAccumulator;
                    entry.component->intervalAccumulator = 0.0f;
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

        for (auto& sys : subsystems)
            sys->update();

        flushDestroyQueue();
    }

    template<typename T, typename... Args>
    T* addObject(Args&&... args) {
        static_assert(std::is_base_of_v<Object, T>, "T MUST derive of Object");

        auto object = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = object.get();
        ptr->scene = this;

        ptr->onComponentAdded = [this, owner = ptr](Component* c) {
            componentPool.push_back({owner, c});
            for (auto& sys : subsystems)
                sys->onComponentAdded(c);
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
        // Notify subsystems before memory is freed.
        for (const auto& entry : componentPool) {
            if (entry.owner->isPendingDestroy() || entry.component->isPendingDestroy())
                for (auto& sys : subsystems)
                    sys->onComponentRemoved(entry.component);
        }

        std::erase_if(componentPool,
            [](const ComponentEntry& e) {
                return e.owner->isPendingDestroy() || e.component->isPendingDestroy();
            });

        for (auto& object : objects)
            object->flushDestroyedComponents();

        std::erase_if(objects,
            [](const std::unique_ptr<Object>& o) { return o->isPendingDestroy(); });
    }

    bool started{false};
    TTF_TextEngine* textEngine{};
    Camera camera;
    Vec2F worldRenderOffset{};

    float physicsAccumulator{0.0f};
    static constexpr float fixedPhysicsStep{1.0f / 60.0f};

    struct ComponentEntry { Object* owner; Component* component; };
    std::vector<ComponentEntry>              componentPool;
    std::vector<std::unique_ptr<Object>>     objects;
    std::vector<Component*>                  renderables;
    std::vector<std::unique_ptr<SceneSubsystem>> subsystems;

    std::string sceneId;
    friend class SceneOrchestrator;

protected:
    template<typename T, typename... Args>
    T* addSubsystem(Args&&... args) {
        static_assert(std::is_base_of_v<SceneSubsystem, T>, "T MUST derive of SceneSubsystem");
        auto sys = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = sys.get();
        subsystems.push_back(std::move(sys));
        return ptr;
    }

    SDL_Color backgroundColor{0,0,0, 0xff};
};

#endif //LETSLEARNSDL_SCENE_H
