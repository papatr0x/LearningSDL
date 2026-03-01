// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_FONTMANAGER_H
#define LETSLEARNSDL_FONTMANAGER_H

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_log.h>
#include <string>
#include <unordered_map>
#include <memory>

class FontManager {
public:
    FontManager(const FontManager&)            = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager(FontManager&&)                 = delete;
    FontManager& operator=(FontManager&&)      = delete;

    static FontManager& instance() {
        static FontManager inst;
        return inst;
    }

    // ptsize is in points (e.g. 16.f for 16pt).
    TTF_Font* load(const std::string& path, const float ptSize) {
        const std::string key = path + "@" + std::to_string(static_cast<int>(ptSize));
        const auto it = fonts.find(key);
        if (it != fonts.end())
            return it->second.get();

        TTF_Font* font = TTF_OpenFont(path.c_str(), ptSize);
        if (!font) {
            SDL_Log("FontManager::load - failed to open '%s': %s", path.c_str(), SDL_GetError());
            return nullptr;
        }
        fonts.emplace(key, FontPtr(font));
        return font;
    }

    void unloadAll() { fonts.clear(); }

private:
    FontManager() = default;

    struct FontDeleter { void operator()(TTF_Font* f) const { TTF_CloseFont(f); } };
    using FontPtr = std::unique_ptr<TTF_Font, FontDeleter>;
    std::unordered_map<std::string, FontPtr> fonts;
};

#endif //LETSLEARNSDL_FONTMANAGER_H
