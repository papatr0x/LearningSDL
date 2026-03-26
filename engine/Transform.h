// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_TRANSFORM_H
#define LETSLEARNSDL_TRANSFORM_H
#include "Vector.h"

struct Transform {
    static Transform make_position(const float x, const float y) {
        return Transform{Vec2F{x, y}};
    }
    Vec2F position{};
    // Vec2F scale{};
    float rotation{};
};

#endif //LETSLEARNSDL_TRANSFORM_H