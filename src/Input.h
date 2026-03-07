// Created by Patricio Palma on 06-03-26.

#ifndef LETSLEARNSDL_INPUT_H
#define LETSLEARNSDL_INPUT_H
#include "Vector.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <array>
#include <cmath>

class Input {
public:
    // No copy, no move
    Input(const Input&)            = delete;
    Input& operator=(const Input&) = delete;
    Input(Input&&)                 = delete;
    Input& operator=(Input&&)      = delete;

    Input()  = default;
    ~Input() { if (gamepad) SDL_CloseGamepad(gamepad); }

    // Call on start, before any SDL events
    void beginFrame() {
        prevKeyState        = currKeyState;
        prevMouseState      = currMouseState;
        prevGamepadState    = currGamepadState;
        mouseDelta          = Vec2F::zero();
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
                mouseDelta.x    += event.motion.xrel;
                mouseDelta.y    += event.motion.yrel;
                break;

            case SDL_EVENT_GAMEPAD_ADDED:
                if (!gamepad)
                    gamepad = SDL_OpenGamepad(event.gdevice.which);
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                if (gamepad && SDL_GetGamepadID(gamepad) == event.gdevice.which) {
                    SDL_CloseGamepad(gamepad);
                    gamepad = nullptr;
                    currGamepadState.fill(false);
                    axisState.fill(0.0f);
                }
                break;

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                currGamepadState[event.gbutton.button] = true;
                break;

            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                currGamepadState[event.gbutton.button] = false;
                break;

            case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
                const float raw{event.gaxis.value / axisMaxValue};
                axisState[event.gaxis.axis] = (std::abs(raw) > deadZone) ? raw : 0.0f;
                break;
            }

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

    // Gamepad
    bool isGamepadConnected() const { return gamepad != nullptr; }

    bool isGamepadButtonDown(const SDL_GamepadButton button) const {
        return currGamepadState[button];
    }

    bool isGamepadButtonPressed(const SDL_GamepadButton button) const {
        return currGamepadState[button] && !prevGamepadState[button];
    }

    bool isGamepadButtonReleased(const SDL_GamepadButton button) const {
        return !currGamepadState[button] && prevGamepadState[button];
    }

    float getAxis(const SDL_GamepadAxis axis) const { return axisState[axis]; }

    void setDeadZone(const float value) { deadZone = value; }

private:
    std::array<bool, SDL_SCANCODE_COUNT> currKeyState{};
    std::array<bool, SDL_SCANCODE_COUNT> prevKeyState{};

    uint8_t currMouseState{};
    uint8_t prevMouseState{};

    Vec2F mousePosition{};
    Vec2F mouseDelta{};

    SDL_Gamepad* gamepad{};
    std::array<bool, SDL_GAMEPAD_BUTTON_COUNT> currGamepadState{};
    std::array<bool, SDL_GAMEPAD_BUTTON_COUNT> prevGamepadState{};
    std::array<float, SDL_GAMEPAD_AXIS_COUNT> axisState{};
    float deadZone{0.15f};
    static constexpr float axisMaxValue{32767.0f};
};

#endif //LETSLEARNSDL_INPUT_H