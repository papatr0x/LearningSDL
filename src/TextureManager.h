// Created by Patricio Palma on 03-03-26.

#ifndef LETSLEARNSDL_TEXTUREMANAGER_H
#define LETSLEARNSDL_TEXTUREMANAGER_H
#include "Texture.h"
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

class TextureManager {
public:
    // Not copyable or movable - sole owner of all textures
    TextureManager(const TextureManager&)            = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&&)                 = delete;
    TextureManager& operator=(TextureManager&&)      = delete;

    ~TextureManager() = default;

    static TextureManager& instance() {
        static TextureManager instance;
        return instance;
    }

    Texture* load(SDL_Renderer* renderer, const std::string& path) {
        auto it = textures.find(path);
        if (it != textures.end()) {
            return it->second.get();
        }

        auto texture = std::make_unique<Texture>();
        if (!texture->load(renderer, path)) {
            SDL_Log("TextureManager::load - failed to load '%s'", path.c_str());
            return nullptr;
        }

        Texture* ptr = texture.get();
        textures.emplace(path, std::move(texture));
        return ptr;
    }

    void unload(const std::string& path) {
        auto it = textures.find(path);
        if (it == textures.end()) {
            SDL_Log("TextureManager::unload - '%s' not found in cache", path.c_str());
            return;
        }
        textures.erase(it);
    }

    void unloadAll() {
        textures.clear();
    }

    bool isLoaded(const std::string& path) const {
        return textures.find(path) != textures.end();
    }

private:
    TextureManager()  = default;
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
};

#endif //LETSLEARNSDL_TEXTUREMANAGER_H