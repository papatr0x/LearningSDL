// Created by Patricio Palma on 07-03-26.

#ifndef LETSLEARNSDL_INPUTCOMPONENT_H
#define LETSLEARNSDL_INPUTCOMPONENT_H
#include "Component.h"
#include "Input.h"
#include "SDL3/SDL_keycode.h"

class InputComponent : public Component {
public:
    explicit InputComponent(Object* owner, const std::string& name, Input& input)
        : Component(owner, name), input(input) {}

    // Keyboard
    bool isKeyDown(const SDL_Keycode key)     const { return input.isKeyDown(key); }
    bool isKeyPressed(const SDL_Keycode key)  const { return input.isKeyPressed(key); }
    bool isKeyReleased(const SDL_Keycode key) const { return input.isKeyReleased(key); }

    // Mouse
    bool isMouseButtonDown(const Uint8 button)     const { return input.isMouseButtonDown(button); }
    bool isMouseButtonPressed(const Uint8 button)  const { return input.isMouseButtonPressed(button); }
    bool isMouseButtonReleased(const Uint8 button) const { return input.isMouseButtonReleased(button); }

    const Vector2<float>& getMousePosition() const { return input.getMousePosition(); }
    const Vector2<float>& getMouseDelta()    const { return input.getMouseDelta(); }

private:
    Input& input;  // non-owning reference, Input lives in Game
};

#endif //LETSLEARNSDL_INPUTCOMPONENT_H