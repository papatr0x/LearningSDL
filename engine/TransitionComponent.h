// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_TRANSITIONCOMPONENT_H
#define LETSLEARNSDL_TRANSITIONCOMPONENT_H

#include "Component.h"
#include "SceneOrchestrator.h"
#include <string>

class TransitionComponent : public Component {
public:
    // targetId empty = completePendingTransition().
    explicit TransitionComponent(Object* owner, const std::string& id, std::string targetId = {})
        : Component(owner, id), targetId(std::move(targetId)) {}

    void trigger() {
        if (triggered) return;
        triggered = true;
        if (targetId.empty())
            SceneOrchestrator::instance().completePendingTransition();
        else
            SceneOrchestrator::instance().goTo(targetId);
    }

private:
    std::string targetId;
    bool        triggered{false};
};

#endif //LETSLEARNSDL_TRANSITIONCOMPONENT_H
