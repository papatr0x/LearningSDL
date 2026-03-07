// Created by Patricio Palma on 28-02-26.

#ifndef LETSLEARNSDL_GAME_H
#define LETSLEARNSDL_GAME_H
#include "Object.h"
#include <SDL3/SDL.h>
#include <vector>

#include "Input.h"

class GameEngine {
public:
    // No copy allowed
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    static GameEngine& instance();

    void init(const char* title, int width, int height);
    void render();
    Input& getInput() { return input; }
    void update(float deltaTime);
    void handleEvents();
    void shutdown() const;
    bool running() const { return m_running;}

    template<typename T, typename ... Args>
    T* addObjectToScene(Args&&... args) {
        static_assert(std::is_base_of_v<Object, T>, "T MUST derive of Object");

        auto object = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = object.get();

        // Every component added to this object is registered in the flat pool.
        ptr->onComponentAdded = [this, owner = ptr](Component* c) {
            m_componentPool.push_back({owner, c});
        };

        m_sceneObjects.push_back(std::move(object));
        return ptr;
    }

    SDL_Renderer* getRenderer() const { return m_renderer; }
    SDL_Window* getWindow() const { return m_window; }

private:
    GameEngine() = default;

    struct ComponentEntry { Object* owner; Component* component; };

    SDL_Window* m_window{};
    SDL_Renderer* m_renderer{};
    bool m_running{false};
    std::vector<std::unique_ptr<Object>> m_sceneObjects;
    std::vector<ComponentEntry> m_componentPool;
    Input input{};
};


#endif //LETSLEARNSDL_GAME_H