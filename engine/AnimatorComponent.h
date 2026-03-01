// Created by Patricio Palma on 07-03-26.

#ifndef LETSLEARNSDL_ANIMATORCOMPONENT_H
#define LETSLEARNSDL_ANIMATORCOMPONENT_H
#include "Component.h"
#include "Animation.h"
#include "RenderComponent.h"
#include <string>
#include <string_view>
#include <unordered_map>
#include <SDL3/SDL_log.h>

class AnimatorComponent : public Component {
public:
    explicit AnimatorComponent(Object* owner, const std::string& id, RenderComponent* renderComp)
        : Component(owner, id), renderComp(renderComp) {}

    // Register a named animation. Overwrites if the name already exists.
    void addAnimation(Animation animation) {
        auto key = animation.name;  // copy name before animation is moved-from
        animations.insert_or_assign(std::move(key), std::move(animation));
    }

    // Start playing a named animation from the beginning.
    // No-op if the animation is already playing.
    void play(std::string_view name) {
        const auto it{animations.find(name)};
        if (it == animations.end()) {
            SDL_Log("AnimatorComponent::play - animation '%s' not found", std::string(name).c_str());
            return;
        }
        if (current == &it->second) return;
        current      = &it->second;
        elapsed      = 0.0f;
        currentFrame = 0;
        playing      = true;
        applyFrame();
    }

    // Stop playback and clear the source rect on the RenderComponent.
    void stop() {
        playing = false;
        current = nullptr;
        if (renderComp) renderComp->clearSourceRect();
    }

    bool isPlaying() const { return playing; }
    std::string_view currentAnimationName() const { return current ? std::string_view{current->name} : std::string_view{}; }

    void updateComponent(float dt) noexcept override {
        if (!playing || !current || !renderComp) return;

        const float frameDuration{1.0f / current->fps};
        elapsed += dt;

        while (elapsed >= frameDuration) {
            elapsed -= frameDuration;
            ++currentFrame;
            if (currentFrame >= static_cast<int>(current->frames.size())) {
                if (current->loop) {
                    currentFrame = 0;
                } else {
                    currentFrame = static_cast<int>(current->frames.size()) - 1;
                    playing = false;
                    break;
                }
            }
        }

        applyFrame();
    }

private:
    void applyFrame() {
        if (renderComp && current && currentFrame < static_cast<int>(current->frames.size()))
            renderComp->setSourceRect(current->frames[currentFrame]);
    }

    struct StringHash {
        using is_transparent = void;
        size_t operator()(std::string_view sv) const {
            return std::hash<std::string_view>{}(sv);
        }
    };

    RenderComponent* renderComp{};
    std::unordered_map<std::string, Animation, StringHash, std::equal_to<>> animations;
    const Animation* current{};
    float elapsed{0.0f};
    int currentFrame{0};
    bool playing{false};
};

#endif //LETSLEARNSDL_ANIMATORCOMPONENT_H
