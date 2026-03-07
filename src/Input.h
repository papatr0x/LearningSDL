// Created by Patricio Palma on 06-03-26.

#ifndef LETSLEARNSDL_INPUT_H
#define LETSLEARNSDL_INPUT_H
#include "Vector.h"
#include <SDL3/SDL_events.h>
#include <unordered_set>

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
                    currKeyState.insert(event.key.key);
                break;

            case SDL_EVENT_KEY_UP:
                currKeyState.erase(event.key.key);
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                currMouseState.insert(event.button.button);
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                currMouseState.erase(event.button.button);
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
    bool isKeyDown(const SDL_Keycode key) const {
        return currKeyState.contains(key);
    }
    
    bool isKeyPressed(const SDL_Keycode key) const {
        return currKeyState.contains(key) &&
               !prevKeyState.contains(key);
    }

    bool isKeyReleased(const SDL_Keycode key) const {
        return !currKeyState.contains(key) &&
               prevKeyState.contains(key);
    }

    // Mouse
    bool isMouseButtonDown(const Uint8 button) const {
        return currMouseState.contains(button);
    }

    bool isMouseButtonPressed(const Uint8 button) const {
        return currMouseState.contains(button) &&
               !prevMouseState.contains(button);
    }

    bool isMouseButtonReleased(const Uint8 button) const {
        return !currMouseState.contains(button) &&
               prevMouseState.contains(button);
    }

    const Vec2F& getMousePosition() const { return mousePosition; }
    const Vec2F& getMouseDelta()    const { return mouseDelta; }

private:
    std::unordered_set<SDL_Keycode> currKeyState;
    std::unordered_set<SDL_Keycode> prevKeyState;

    std::unordered_set<Uint8> currMouseState;
    std::unordered_set<Uint8> prevMouseState;

    Vec2F mousePosition{};
    Vec2F mouseDelta{};
};

#endif //LETSLEARNSDL_INPUT_H