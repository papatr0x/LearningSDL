// Created by Patricio Palma on 08-03-26.

#include "Scene.h"
#include "CollisionSystem.h"
#include "GameEngine.h"

Scene::Scene() {
    componentPool.reserve(256);
    textEngine = GameEngine::instance().getTextEngine();
    // Default camera position to screen center so that world coordinate (0,0) maps to
    // the top-left corner — preserving the existing screen-coordinate convention.
    int w{}, h{};
    SDL_GetRenderOutputSize(GameEngine::instance().getRenderer(), &w, &h);
    camera.position = { w * 0.5f, h * 0.5f };
}
