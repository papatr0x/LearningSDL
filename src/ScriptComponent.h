// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_SCRIPTCOMPONENT_H
#define LETSLEARNSDL_SCRIPTCOMPONENT_H

#include "Component.h"
#include <SDL3/SDL.h>
#include <functional>

class ScriptComponent : public Component {
public:
    // UpdateFn receives the owner so scripts can call getComponent<T>()
    using UpdateFn = std::function<void(float dt, Object* owner)>;

    explicit ScriptComponent(Object* owner, const std::string& name, UpdateFn fn) : Component
    (owner, name), updateFn(std::move(fn))
    {
        if (!updateFn)
            SDL_Log("ScriptComponent - update function is null");
        if (!owner)
            SDL_Log("ScriptComponent - owner ptr is null");
    }

    void update(const float dt) noexcept override {
        if (!isEnabled() || !updateFn) return;
        updateFn(dt, owner);
    }

private:
    UpdateFn updateFn;
};

#endif //LETSLEARNSDL_SCRIPTCOMPONENT_H
