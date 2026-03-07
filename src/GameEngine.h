// Created by Patricio Palma on 28-02-26.

#ifndef LETSLEARNSDL_GAME_H
#define LETSLEARNSDL_GAME_H
#include "Input.h"
#include "Scene.h"
#include <SDL3/SDL.h>

class GameEngine {
public:
    // No copy allowed
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    static GameEngine& instance();

    bool init(const char* title, int width, int height);
    void handleEvents();
    void shutdown();
    void run();

    template<typename T, typename... Args>
    void loadScene(Args&&... args) {
        static_assert(std::is_base_of_v<Scene, T>, "T MUST derive from Scene");
        pendingScene = std::make_unique<T>(std::forward<Args>(args)...);
    }

    Input& getInput() { return input; }
    SDL_Renderer* getRenderer() const { return renderer; }
    SDL_Window* getWindow() const { return window; }

private:
    GameEngine() = default;

    SDL_Window* window{};
    SDL_Renderer* renderer{};
    bool running{false};
    Input input{};
    std::unique_ptr<Scene> activeScene;
    std::unique_ptr<Scene> pendingScene;
};


#endif //LETSLEARNSDL_GAME_H