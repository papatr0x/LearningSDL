// Created by Patricio Palma on 07-03-26.

#ifndef LETSLEARNSDL_ANIMATION_H
#define LETSLEARNSDL_ANIMATION_H
#include <string>
#include <string_view>
#include <vector>
#include <SDL3/SDL_rect.h>

struct Animation {
    std::string name;
    std::vector<SDL_FRect> frames;
    float fps{12.0f};
    bool loop{true};
};

class AnimationBuilder {
public:
    // All frames in a single horizontal row of the sprite sheet.
    // startX/startY: pixel offset to the first frame.
    static Animation fromStrip(const std::string_view name, const int frameCount,
                               const int frameWidth, const int frameHeight,
                               const int startX = 0, const int startY = 0,
                               const float fps = 12.0f, const bool loop = true) {
        Animation anim;
        anim.name  = std::string(name);
        anim.fps   = fps;
        anim.loop  = loop;
        anim.frames.reserve(frameCount);
        for (int i{0}; i < frameCount; ++i) {
            anim.frames.push_back({
                static_cast<float>(startX + i * frameWidth),
                static_cast<float>(startY),
                static_cast<float>(frameWidth),
                static_cast<float>(frameHeight)
            });
        }
        return anim;
    }

    // Frames arranged in a grid, read left-to-right then top-to-bottom.
    static Animation fromGrid(const std::string_view name, const int cols, const int rows,
                              const int frameWidth, const int frameHeight,
                              const float fps = 12.0f, const bool loop = true) {
        Animation anim;
        anim.name  = std::string(name);
        anim.fps   = fps;
        anim.loop  = loop;
        anim.frames.reserve(cols * rows);
        for (int row{0}; row < rows; ++row) {
            for (int col{0}; col < cols; ++col) {
                anim.frames.push_back({
                    static_cast<float>(col * frameWidth),
                    static_cast<float>(row * frameHeight),
                    static_cast<float>(frameWidth),
                    static_cast<float>(frameHeight)
                });
            }
        }
        return anim;
    }
};

#endif //LETSLEARNSDL_ANIMATION_H
