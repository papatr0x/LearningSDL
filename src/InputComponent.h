// Created by Patricio Palma on 07-03-26.

#ifndef LETSLEARNSDL_INPUTCOMPONENT_H
#define LETSLEARNSDL_INPUTCOMPONENT_H
#include "Component.h"
#include "Input.h"
#include "InputActions.h"
#include "SDL3/SDL_scancode.h"

class InputComponent : public Component {
public:
    explicit InputComponent(Object* owner, const std::string& name, Input& input)
        : Component(owner, name), input(input) {}

    // Action bindings — keyboard
    void bind(const std::string_view action, const SDL_Scancode scancode) { actions.bind(action, scancode); }
    void unbind(const std::string_view action, const SDL_Scancode scancode) { actions.unbind(action, scancode); }

    // Action bindings — gamepad
    void bind(const std::string_view action, const SDL_GamepadButton button) { actions.bind(action, button); }
    void unbind(const std::string_view action, const SDL_GamepadButton button) { actions.unbind(action, button); }

    void unbindAll(const std::string_view action) { actions.unbindAll(action); }

    // Action queries — checks keyboard and gamepad bindings
    bool isActionDown(const std::string_view action) const {
        const auto* b = actions.getBindings(action);
        if (!b) return false;
        for (const auto key : b->keys)
            if (input.isKeyDown(key)) return true;
        for (const auto btn : b->buttons)
            if (input.isGamepadButtonDown(btn)) return true;
        return false;
    }

    bool isActionPressed(const std::string_view action) const {
        const auto* b = actions.getBindings(action);
        if (!b) return false;
        for (const auto key : b->keys)
            if (input.isKeyPressed(key)) return true;
        for (const auto btn : b->buttons)
            if (input.isGamepadButtonPressed(btn)) return true;
        return false;
    }

    bool isActionReleased(const std::string_view action) const {
        const auto* b = actions.getBindings(action);
        if (!b) return false;
        for (const auto key : b->keys)
            if (input.isKeyReleased(key)) return true;
        for (const auto btn : b->buttons)
            if (input.isGamepadButtonReleased(btn)) return true;
        return false;
    }

    // Keyboard raw
    bool isKeyDown(const SDL_Scancode key) const { return input.isKeyDown(key); }
    bool isKeyPressed(const SDL_Scancode key) const { return input.isKeyPressed(key); }
    bool isKeyReleased(const SDL_Scancode key) const { return input.isKeyReleased(key); }

    // Gamepad raw
    bool isGamepadButtonDown(const SDL_GamepadButton btn) const { return input.isGamepadButtonDown(btn); }
    bool isGamepadButtonPressed(const SDL_GamepadButton btn) const { return input.isGamepadButtonPressed(btn); }
    bool isGamepadButtonReleased(const SDL_GamepadButton btn) const { return input.isGamepadButtonReleased(btn); }
    float getAxis(const SDL_GamepadAxis axis) const { return input.getAxis(axis); }

    // Mouse
    bool isMouseButtonDown(const Uint8 button) const { return input.isMouseButtonDown(button); }
    bool isMouseButtonPressed(const Uint8 button) const { return input.isMouseButtonPressed(button); }
    bool isMouseButtonReleased(const Uint8 button) const { return input.isMouseButtonReleased(button); }

    const Vec2F& getMousePosition() const { return input.getMousePosition(); }
    const Vec2F& getMouseDelta() const { return input.getMouseDelta(); }

private:
    Input& input;        // non-owning reference, Input lives in GameEngine
    InputActions actions; // per-component action bindings
};

#endif //LETSLEARNSDL_INPUTCOMPONENT_H