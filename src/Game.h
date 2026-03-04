// Created by Patricio Palma on 28-02-26.

#ifndef LETSLEARNSDL_GAME_H
#define LETSLEARNSDL_GAME_H

#include <SDL3/SDL.h>

#include "Player.h"

class Game {
public:
    void init(const char* title, int width, int height);
    void render();
    void update(float deltaTime);
    void handleEvents();
    void shutdown() const;

    bool running() const { return m_running;}
private:
    SDL_Window* m_window{};
    SDL_Renderer* m_renderer{};
    bool m_running{false};
    // int m_currentFrame{0};
    Player m_player;
    Player m_go;
};


#endif //LETSLEARNSDL_GAME_H