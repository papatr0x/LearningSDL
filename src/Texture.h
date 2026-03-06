// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_TEXTURE_H
#define LETSLEARNSDL_TEXTURE_H
#include <string>
#include <SDL3/SDL.h>

class Texture {
public:
    Texture() = default;

    // No copyable - avoids double free on SDL_Texture*
    Texture(const Texture&)            = delete;
    Texture& operator=(const Texture&) = delete;

    // Movable
    Texture(Texture&& other) noexcept
        : sdlTexture(other.sdlTexture)
        , width(other.width)
        , height(other.height)
    {
        other.sdlTexture = nullptr;
        other.width      = 0;
        other.height     = 0;
    }

    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            destroy();
            sdlTexture       = other.sdlTexture;
            width            = other.width;
            height           = other.height;
            other.sdlTexture = nullptr;
            other.width      = 0;
            other.height     = 0;
        }
        return *this;
    }

    ~Texture() { destroy(); }

    bool load(SDL_Renderer* renderer, const std::string& path) {
        if (!renderer) {
            SDL_Log("Texture::load - renderer is nullptr");
            return false;
        }
        if (path.empty()) {
            SDL_Log("Texture::load - path is empty");
            return false;
        }
        SDL_Surface* surface = SDL_LoadPNG(path.c_str());
        if (!surface) {
            SDL_Log("Texture::load - failed to load texture '%s': %s", path.c_str(), SDL_GetError());
            return false;
        }

        width  = surface->w;
        height = surface->h;

        sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (!sdlTexture) {
            SDL_Log("Texture::load - failed to load texture '%s': %s", path.c_str(), SDL_GetError());
            width  = 0;
            height = 0;
            return false;
        }

        return true;
    }

    bool isValid() const { return sdlTexture != nullptr; }
    SDL_Texture* get()    const { return sdlTexture; }
    int  getWidth()  const { return width; }
    int  getHeight() const { return height; }

private:
    void destroy() {
        if (sdlTexture) {
            SDL_DestroyTexture(sdlTexture);
            sdlTexture = nullptr;
        }
    }

    SDL_Texture* sdlTexture{};
    int width{};
    int height{};
};

#endif //LETSLEARNSDL_TEXTURE_H