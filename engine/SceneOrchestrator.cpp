// Created by Patricio Palma on 08-03-26.

#include "SceneOrchestrator.h"
#include "GameEngine.h"
#include <SDL3/SDL_log.h>

void SceneOrchestrator::setTransition(const std::string& from, const std::string& to, const std::string& via) {
    if (!via.empty())
        transitionMap[{from, to}] = via;
}

void SceneOrchestrator::start(const std::string& id) {
    doLoad(id);
}

void SceneOrchestrator::goTo(const std::string& targetId) {
    const auto* active = GameEngine::instance().getActiveScene();
    const std::string fromId = active ? active->getSceneId() : std::string{};
    const auto it = transitionMap.find({fromId, targetId});
    if (it != transitionMap.end()) {
        pendingId = targetId;
        doLoad(it->second);
    } else {
        doLoad(targetId);
    }
}

const std::string& SceneOrchestrator::getCurrentSceneId() const {
    const auto* active = GameEngine::instance().getActiveScene();
    static const std::string empty{};
    return active ? active->getSceneId() : empty;
}

void SceneOrchestrator::completePendingTransition() {
    if (pendingId.empty()) {
        SDL_Log("SceneOrchestrator: completePendingTransition called with no pending target");
        return;
    }
    std::string target = std::move(pendingId);
    doLoad(target);
}

void SceneOrchestrator::doLoad(const std::string& id) {
    const auto it = registry.find(id);
    if (it == registry.end()) {
        SDL_Log("SceneOrchestrator: scene '%s' is not registered", id.c_str());
        return;
    }
    auto scene = it->second();
    scene->sceneId = id;
    GameEngine::instance().loadScene(std::move(scene));
}
