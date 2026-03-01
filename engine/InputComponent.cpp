// Created by Patricio Palma on 10-03-26.

#include "Object.h"
#include "InputComponent.h"
#include "GameEngine.h"

InputComponent::InputComponent(Object* owner, const std::string& id)
    : Component(owner, id), input(GameEngine::instance().getInput()) {}
