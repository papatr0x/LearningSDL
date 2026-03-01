// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_TIMERCOMPONENT_H
#define LETSLEARNSDL_TIMERCOMPONENT_H

#include "Component.h"
#include <functional>

class TimerComponent : public Component {
public:
    using Callback = std::function<void()>;

    TimerComponent(Object* owner, const std::string& id, const float duration, Callback onComplete)
        : Component(owner, id), duration(duration), onComplete(std::move(onComplete)) {}

    void updateComponent(const float dt) noexcept override {
        elapsed += dt;
        if (elapsed >= duration) {
            elapsed = 0.f;
            setEnabled(false);
            if (onComplete) onComplete();
        }
    }

private:
    float    duration;
    float    elapsed{0.f};
    Callback onComplete;
};

#endif //LETSLEARNSDL_TIMERCOMPONENT_H
