// Created by Patricio Palma on 03-03-26.

#include "TextureManager.h"
#include <iostream>

void TextureManager::drawFrame(const std::string& id, const float x, const float y,
    const float width, const float height,  const int currentRow,
    const int currentFrame, SDL_Renderer* renderer, SDL_FlipMode flip) {

    SDL_FRect srcRect{};
    SDL_FRect destRect{};
    SDL_FPoint center{};

    srcRect.x = width * currentFrame;
    srcRect.y = height * (currentRow - 1);
    srcRect.w = width;
    srcRect.h = height;
    destRect.w = width;
    destRect.h = height;
    destRect.x = x;
    destRect.y = y;
    SDL_RenderTextureRotated(renderer, m_textureMap[id], &srcRect, &destRect, 0.f, &center, flip);

}

void TextureManager::draw(const std::string& id, const float x, const float y, const float width,
                          const float height, SDL_Renderer* renderer, const SDL_FlipMode flip) {
    SDL_FRect src{};
    SDL_FRect dest{};
    SDL_FPoint center{};

    src.x = src.y = 0.f;
    src.w = dest.w = width;
    src.h = dest.h = height;
    dest.x = x;
    dest.y = y;
    SDL_RenderTextureRotated(renderer, m_textureMap[id], &src, &dest, 0.f, &center, flip);
}

bool TextureManager::load(const std::string& filename, const std::string& id, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = SDL_LoadPNG(filename.c_str());
    if (!tempSurface) {
        std::cerr << "failed to load surface for " << filename << "\n";
        return false;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_DestroySurface(tempSurface);
    if (!texture) {
        std::cerr << "failed to create texture for " << filename << "\n";
        return false;
    }
    m_textureMap[id] = texture;
    return true;
}
