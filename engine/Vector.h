// Created by Patricio Palma on 06-03-26.

#ifndef LETSLEARNSDL_VECTOR_H
#define LETSLEARNSDL_VECTOR_H

#include <cmath>

template<typename T>
struct Vector2 {
    T x{}, y{};
    static Vector2<T> zero() { return Vector2<T>{};}

    Vector2 operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
    Vector2 operator-(const Vector2& other) const { return { x - other.x, y - other.y }; }
    Vector2 operator*(T scalar)             const { return { x * scalar,  y * scalar  }; }
    Vector2 operator/(T scalar)             const { return { x / scalar,  y / scalar  }; }

    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
    Vector2& operator*=(T scalar)             { x *= scalar;  y *= scalar;  return *this; }
    Vector2& operator/=(T scalar)             { x /= scalar;  y /= scalar;  return *this; }

    float dot(const Vector2& other) const { return x * other.x + y * other.y; }

    bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vector2& other) const { return !(*this == other); }

    float length() const { return std::sqrt(x * x + y * y); }
    Vector2 normalized() const {
        const float len = length();
        return len > 0.f ? *this / len : Vector2{};
    }
};

// Maybe someday ...
template<typename T>
struct Vector3{
    T x{}, y{}, z{};
    static Vector3<T> zero() { return Vector3<T>{};}

    Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
    Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }
    Vector3 operator*(T scalar)             const { return { x * scalar,  y * scalar,  z * scalar }; }
    Vector3 operator/(T scalar)             const { return { x / scalar,  y / scalar,  z / scalar }; }

    Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vector3& operator*=(T scalar)             { x *= scalar;  y *= scalar;  z *= scalar; return *this; }
    Vector3& operator/=(T scalar)             { x /= scalar;  y /= scalar;  z /= scalar; return *this; }

    float dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }

    bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator!=(const Vector3& other) const { return !(*this == other); }

    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vector3 normalized() const {
        const float len = length();
        return len > 0.f ? *this / len : Vector3{};
    }
};

using Vec2F = Vector2<float>;
using Vec3F = Vector3<float>;
using Vec2 = Vector2<int>;
using Vec3 = Vector3<int>;

#endif //LETSLEARNSDL_VECTOR_H