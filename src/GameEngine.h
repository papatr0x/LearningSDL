// Created by Patricio Palma on 28-02-26.

#ifndef LETSLEARNSDL_GAME_H
#define LETSLEARNSDL_GAME_H
#include "Object.h"
#include <SDL3/SDL.h>
#include <vector>

class GameEngine {
public:
    static GameEngine& instance();

    void engineInit(const char* title, int width, int height);
    void render();
    void update(float deltaTime);
    void handleEvents();
    void shutdown() const;
    bool running() const { return m_running;}

    template<typename T, typename ... Args>
    T* addObjectToScene(Args&&... args) {
        static_assert(std::is_base_of_v<Object, T>, "T MUST derive of Object");

        auto object = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = object.get();
        m_sceneObjects.push_back(std::move(object));
        return ptr;
    }

private:
    GameEngine() = default;
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;
    SDL_Window* m_window{};
    SDL_Renderer* m_renderer{};
    bool m_running{false};

    std::vector<std::unique_ptr<Object>> m_sceneObjects;
};


#endif //LETSLEARNSDL_GAME_H