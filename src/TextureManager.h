// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_TEXTUREMANAGER_H
#define LETSLEARNSDL_TEXTUREMANAGER_H
#include <string>
#include <unordered_map>
#include "SDL3/SDL.h"

using TextureID = std::string;

struct SDL_Renderer;

class TextureManager {
public:
    static TextureManager& instance() {
        static TextureManager instance;
        return instance;
    }
    void drawFrame(const TextureID& id, const float x, const float y, const float width, const
        float height,  const int currentRow,
    const int currentFrame, SDL_Renderer* renderer, SDL_FlipMode flip = SDL_FLIP_NONE);

    void draw(const TextureID& id, const float x,  const float y, const float width, const float height, SDL_Renderer* renderer,
    const SDL_FlipMode flip = SDL_FLIP_NONE);

    bool load(const std::string& filename, const TextureID& id, SDL_Renderer* renderer);

protected:
    TextureManager() = default;
    std::unordered_map<TextureID, SDL_Texture*> m_textureMap;

};
#endif //LETSLEARNSDL_TEXTUREMANAGER_H