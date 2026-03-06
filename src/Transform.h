// Created by Patricio Palma on 05-03-26.

#ifndef LETSLEARNSDL_TRANSFORM_H
#define LETSLEARNSDL_TRANSFORM_H

struct Transform {
    static Transform make_position(float x, float y) {
        return Transform{x, y};
    }
    static Transform make_area(float width, float height) {
        return Transform{0, 0, width, height};
    }
    float x{}, y{};
    float width{}, height{};
    float scaleX{1.0f}, scaleY{1.0f};
    float rotation{};
};

#endif //LETSLEARNSDL_TRANSFORM_H