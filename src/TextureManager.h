// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_TEXTUREMANAGER_H
#define LETSLEARNSDL_TEXTUREMANAGER_H
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

class TextureManager {
public:
    static TextureManager& instance() {
        static TextureManager instance;
        return instance;
    }
    void drawFrame(const std::string& id, const float x, const float y, const float width, const
        float height,  const int currentRow,
    const int currentFrame, SDL_Renderer* renderer, SDL_FlipMode flip = SDL_FLIP_NONE);

    void draw(const std::string& id, const float x,  const float y, const float width, const float height, SDL_Renderer* renderer,
    const SDL_FlipMode flip = SDL_FLIP_NONE);

    bool load(const std::string& filename, const std::string& id, SDL_Renderer* renderer);

protected:
    TextureManager() = default;
    std::unordered_map<std::string, SDL_Texture*> m_textureMap;

};
#endif //LETSLEARNSDL_TEXTUREMANAGER_H