// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_SCENEORCHESTRATOR_H
#define LETSLEARNSDL_SCENEORCHESTRATOR_H

#include "Scene.h"
#include <string>
#include <unordered_map>
#include <map>
#include <functional>
#include <memory>

class SceneOrchestrator {
public:
    SceneOrchestrator(const SceneOrchestrator&)            = delete;
    SceneOrchestrator& operator=(const SceneOrchestrator&) = delete;

    static SceneOrchestrator& instance() {
        static SceneOrchestrator inst;
        return inst;
    }

    template<typename T>
    void registerScene(const std::string& id) {
        static_assert(std::is_base_of_v<Scene, T>, "T MUST derive from Scene");
        registry[id] = []() { return std::make_unique<T>(); };
    }

    // Declares a transition rule from→to.
    // If 'via' is provided, that scene is shown before loading the target.
    void setTransition(const std::string& from, const std::string& to, const std::string& via = {});

    // Loads the initial scene.
    void start(const std::string& id);

    // Requests a transition to targetId.
    // If a 'via' scene is registered for (current→target), it loads that first.
    void goTo(const std::string& targetId);

    // Called by the intermediate scene when it finishes, loads the pending target.
    void completePendingTransition();

    const std::string& getCurrentSceneId() const;

private:
    SceneOrchestrator() = default;

    using SceneFactory = std::function<std::unique_ptr<Scene>()>;
    std::unordered_map<std::string, SceneFactory> registry;
    std::map<std::pair<std::string, std::string>, std::string> transitionMap;
    std::string pendingId;

    void doLoad(const std::string& id);
};

#endif //LETSLEARNSDL_SCENEORCHESTRATOR_H
