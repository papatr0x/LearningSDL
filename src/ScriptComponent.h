// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_SCRIPTCOMPONENT_H
#define LETSLEARNSDL_SCRIPTCOMPONENT_H

#include "Component.h"
#include <SDL3/SDL.h>
#include <functional>

class ScriptComponent : public Component {
public:
    using UpdateFn = std::function<void(float dt)>;

    explicit ScriptComponent(const std::string& name, UpdateFn fn)
        : Component(name), updateFn(std::move(fn))
    {
        if (!updateFn) {
            SDL_Log("ScriptComponent - updateFn is nullptr");
        }
    }

    void update(float dt) noexcept override {
        if (!isEnabled()) return;
        if (!updateFn)    return;
        updateFn(dt);
    }

private:
    UpdateFn updateFn;
};

#endif //LETSLEARNSDL_SCRIPTCOMPONENT_H