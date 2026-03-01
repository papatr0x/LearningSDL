// Created by Patricio Palma on 28-02-26.

#ifndef LETSLEARNSDL_GAME_H
#define LETSLEARNSDL_GAME_H

#include <string>
#include <SDL3/SDL.h>

class Game {
public:
    void init(const char* title, int width, int height);
    void render();
    void update();
    void handleEvents();
    void shutdown() const;

    void load_texture();

    bool running() const { return m_running;}
private:
    SDL_Window* m_window{};
    SDL_Renderer* m_renderer{};
    bool m_running{false};
    SDL_Texture* m_texture{};
    SDL_FRect m_src_rect{}, m_dest_rect{};
};


#endif //LETSLEARNSDL_GAME_H