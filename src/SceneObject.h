// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_SCENEOBJECT_H
#define LETSLEARNSDL_SCENEOBJECT_H

#include "Object.h"
#include "Transform.h"

class SceneObject : public Object {
public:
    explicit SceneObject(std::string name) : Object(std::move(name)) { }
    void setTransform(const Transform& t) {
        transform = t;
    }
    Transform* getTransform() {
        return &transform;
    }
protected:
    Transform transform{};
};

#endif //LETSLEARNSDL_SCENEOBJECT_H
