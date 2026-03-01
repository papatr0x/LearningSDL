// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_SCRIPTCOMPONENT_H
#define LETSLEARNSDL_SCRIPTCOMPONENT_H

#include "Component.h"
#include <SDL3/SDL.h>
#include <functional>

class ScriptComponent : public Component {
public:
    // UpdateFn receives the owner so scripts can call getComponent<T>()
    using StartFn  = std::function<void(Object* owner)>;
    using UpdateFn = std::function<void(float dt, Object* owner)>;

    explicit ScriptComponent(Object* owner, const std::string& name, UpdateFn update, StartFn start = nullptr)
        : Component(owner, name), updateFn(std::move(update)), startFn(std::move(start))
    {
        if (!updateFn)
            SDL_Log("ScriptComponent - update function is null");
        if (!owner)
            SDL_Log("ScriptComponent - owner ptr is null");
    }

    void onStart() noexcept override {
        if (startFn) startFn(owner);
    }

    void updateComponent(const float dt) noexcept override {
        if (!updateFn) return;
        updateFn(dt, owner);
    }

private:
    UpdateFn updateFn;
    StartFn  startFn;
};

#endif //LETSLEARNSDL_SCRIPTCOMPONENT_H
