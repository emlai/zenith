#pragma once

#include "math.h"
#include <cassert>
#include <cmath>

enum Dir8
{
    North,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest
};

const int directions = 8;

template<typename T>
struct Vector2Base;

using Vector2 = Vector2Base<int>;
using Vector2f = Vector2Base<double>;

extern const Vector2 directionVectors[]; // Definition in math.cpp.

/// 2D vector structure for representing values with x and y components.
template<typename T>
struct Vector2Base
{
    T x, y;

    Vector2Base() = default;
    constexpr Vector2Base(T x, T y) : x(x), y(y) {}
    constexpr Vector2Base(Dir8 direction) : Vector2Base(directionVectors[direction]) {}
    template<typename U>
    explicit constexpr Vector2Base(Vector2Base<U> vector) : x(T(vector.x)), y(T(vector.y)) {}

    Vector2Base& operator+=(Vector2Base vector) { x += vector.x; y += vector.y; return *this; }
    Vector2Base& operator-=(Vector2Base vector) { x -= vector.x; y -= vector.y; return *this; }
    Vector2Base& operator*=(Vector2Base vector) { x *= vector.x; y *= vector.y; return *this; }
    Vector2Base& operator/=(Vector2Base vector) { x /= vector.x; y /= vector.y; return *this; }

    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base& operator*=(U multiplier) { x *= multiplier; y *= multiplier; return *this; }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base& operator/=(U divisor) { x /= divisor; y /= divisor; return *this; }

    Vector2Base operator+(Vector2Base vector) const { return Vector2Base(x + vector.x, y + vector.y); }
    Vector2Base operator-(Vector2Base vector) const { return Vector2Base(x - vector.x, y - vector.y); }
    Vector2Base operator*(Vector2Base vector) const { return Vector2Base(x * vector.x, y * vector.y); }
    Vector2Base operator/(Vector2Base vector) const { return Vector2Base(x / vector.x, y / vector.y); }

    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator*(U multiplier) const { return Vector2Base(T(x * multiplier), T(y * multiplier)); }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator/(U divisor) const { return Vector2Base(T(x / divisor), T(y / divisor)); }

    Vector2Base operator+() const { return *this; }
    Vector2Base operator-() const { return Vector2Base(-x, -y); }

    bool operator==(Vector2Base vector) const { return x == vector.x && y == vector.y; }
    bool operator!=(Vector2Base vector) const { return x != vector.x || y != vector.y; }

    auto getLength() const { return std::sqrt(getLengthSquared()); }
    auto getLengthSquared() const { return x * x + y * y; }
    auto getArea() const { return x * y; }
    bool isZero() const { return x == 0 && y == 0; }
    template<typename U>
    bool isWithin(Vector2Base<U>) const;
    bool isWithin(struct Rect) const;

    static const Vector2Base zeroVector;
};

template<typename T>
template<typename U>
inline bool Vector2Base<T>::isWithin(Vector2Base<U> vector) const
{
    return x >= 0 && U(x) < vector.x && y >= 0 && U(y) < vector.y;
}

template<typename T>
const Vector2Base<T> Vector2Base<T>::zeroVector = Vector2Base<T>(0, 0);

template<typename T>
inline auto getDistanceSquared(Vector2Base<T> a, Vector2Base<T> b)
{
    return (b - a).getLengthSquared();
}

template<typename T>
inline auto getDistance(Vector2Base<T> a, Vector2Base<T> b)
{
    return std::sqrt(getDistanceSquared(a, b));
}

template<typename T>
inline Vector2Base<T> makeRandomVector(Vector2Base<T> max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(max.x), randInt(max.y) };
}

template<typename T>
inline Vector2Base<T> makeRandomVector(Vector2Base<T> min, Vector2Base<T> max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(min.x, max.x), randInt(min.y, max.y) };
}

struct Rect
{
    Vector2 position; ///< Top-left corner
    Vector2 size;

    Rect() = default;
    constexpr Rect(Vector2 position, Vector2 size) : position(position), size(size) {}
    constexpr Rect(int x, int y, int w, int h) : position(x, y), size(w, h) {}

    int getLeft() const { return position.x; }
    int getRight() const { return position.x + size.x; }
    int getTop() const { return position.y; }
    int getBottom() const { return position.y + size.y; }
    Vector2 getCenter() const { return position + size / 2; }

    int getArea() const { return size.getArea(); }
    int getPerimeter() const { return 2 * (size.x + size.y); }
    bool isSquare() const { return size.x == size.y; }

    Rect offset(Vector2 offset) const { return Rect(position + offset, size); }
};

template<typename T>
inline bool Vector2Base<T>::isWithin(Rect rect) const
{
    return x >= rect.getLeft() && x < rect.getRight() &&
           y >= rect.getTop() && y < rect.getBottom();
}
