// Created by Patricio Palma on 06-03-26.

#ifndef LETSLEARNSDL_INPUT_H
#define LETSLEARNSDL_INPUT_H
#include "Vector.h"
#include <SDL3/SDL_events.h>
#include <array>

class Input {
public:
    // No copy, no move
    Input(const Input&)            = delete;
    Input& operator=(const Input&) = delete;
    Input(Input&&)                 = delete;
    Input& operator=(Input&&)      = delete;

    Input()  = default;
    ~Input() = default;

    // Call on start, before any SDL events
    void beginFrame() {
        prevKeyState  = currKeyState;
        prevMouseState = currMouseState;
        mouseDelta = Vec2F::zero();
    }

    // Call on each SDL_Event on the game loop
    void processEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                if (!event.key.repeat)
                    currKeyState[event.key.scancode] = true;
                break;

            case SDL_EVENT_KEY_UP:
                currKeyState[event.key.scancode] = false;
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                currMouseState |= static_cast<uint8_t>(1 << (event.button.button - 1));
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                currMouseState &= static_cast<uint8_t>(~(1 << (event.button.button - 1)));
                break;

            case SDL_EVENT_MOUSE_MOTION:
                mousePosition.x = event.motion.x;
                mousePosition.y = event.motion.y;
                mouseDelta.x    = event.motion.xrel;
                mouseDelta.y    = event.motion.yrel;
                break;

            default:
                break;
        }
    }
    
    // Keyboard
    bool isKeyDown(const SDL_Scancode key) const {
        return currKeyState[key];
    }

    bool isKeyPressed(const SDL_Scancode key) const {
        return currKeyState[key] && !prevKeyState[key];
    }

    bool isKeyReleased(const SDL_Scancode key) const {
        return !currKeyState[key] && prevKeyState[key];
    }

    // Mouse
    bool isMouseButtonDown(const Uint8 button) const {
        return (currMouseState & (1 << (button - 1))) != 0;
    }

    bool isMouseButtonPressed(const Uint8 button) const {
        return (currMouseState & (1 << (button - 1))) != 0 &&
               (prevMouseState & (1 << (button - 1))) == 0;
    }

    bool isMouseButtonReleased(const Uint8 button) const {
        return (currMouseState & (1 << (button - 1))) == 0 &&
               (prevMouseState & (1 << (button - 1))) != 0;
    }

    const Vec2F& getMousePosition() const { return mousePosition; }
    const Vec2F& getMouseDelta()    const { return mouseDelta; }

private:
    std::array<bool, SDL_SCANCODE_COUNT> currKeyState{};
    std::array<bool, SDL_SCANCODE_COUNT> prevKeyState{};

    uint8_t currMouseState{};
    uint8_t prevMouseState{};

    Vec2F mousePosition{};
    Vec2F mouseDelta{};
};

#endif //LETSLEARNSDL_INPUT_H