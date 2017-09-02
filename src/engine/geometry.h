#pragma once

#include "math.h"
#include "utility.h"
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

inline Dir8 randomDir8()
{
    switch (randInt(7))
    {
        case 0: return North;
        case 1: return NorthEast;
        case 2: return East;
        case 3: return SouthEast;
        case 4: return South;
        case 5: return SouthWest;
        case 6: return West;
        case 7: return NorthWest;
    }

    assert(false);
}

template<typename T>
struct Vector2Base;
template<typename T>
struct Vector3Base;

using Vector2 = Vector2Base<int>;
using Vector2f = Vector2Base<double>;
using Vector3 = Vector3Base<int>;
using Vector3f = Vector3Base<double>;

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
    explicit constexpr Vector2Base(Vector3Base<T> vector) : x(vector.x), y(vector.y) {}

    Vector2Base& operator+=(Vector2Base vector) { x += vector.x; y += vector.y; return *this; }
    Vector2Base& operator-=(Vector2Base vector) { x -= vector.x; y -= vector.y; return *this; }
    Vector2Base& operator*=(Vector2Base vector) { x *= vector.x; y *= vector.y; return *this; }
    Vector2Base& operator/=(Vector2Base vector) { x /= vector.x; y /= vector.y; return *this; }
    Vector2Base& operator%=(Vector2Base vector) { x %= vector.x; y %= vector.y; return *this; }

    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base& operator*=(U multiplier) { x *= multiplier; y *= multiplier; return *this; }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base& operator/=(U divisor) { x /= divisor; y /= divisor; return *this; }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base& operator%=(U divisor) { x %= divisor; y %= divisor; return *this; }

    Vector2Base operator+(Vector2Base vector) const { return Vector2Base(x + vector.x, y + vector.y); }
    Vector2Base operator-(Vector2Base vector) const { return Vector2Base(x - vector.x, y - vector.y); }
    Vector2Base operator*(Vector2Base vector) const { return Vector2Base(x * vector.x, y * vector.y); }
    Vector2Base operator/(Vector2Base vector) const { return Vector2Base(x / vector.x, y / vector.y); }
    Vector2Base operator%(Vector2Base vector) const { return Vector2Base(x % vector.x, y % vector.y); }

    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator*(U multiplier) const { return Vector2Base(T(x * multiplier), T(y * multiplier)); }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator/(U divisor) const { return Vector2Base(T(x / divisor), T(y / divisor)); }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator%(U divisor) const { return Vector2Base(T(x % divisor), T(y % divisor)); }

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

    Vector2 divideRoundingDown(int divisor) const
    {
        return Vector2(::divideRoundingDown(x, divisor), ::divideRoundingDown(y, divisor));
    }

    Dir8 getDir8() const;

    static const Vector2Base zeroVector;
};

template<typename T>
template<typename U>
inline bool Vector2Base<T>::isWithin(Vector2Base<U> vector) const
{
    return x >= 0 && U(x) < vector.x && y >= 0 && U(y) < vector.y;
}

template<typename T>
Dir8 Vector2Base<T>::getDir8() const
{
    switch (sign(x))
    {
        case -1:
            switch (sign(y))
            {
                case -1: return NorthWest;
                case 0: return West;
                case 1: return SouthWest;
            }
        case 0:
            switch (sign(y))
            {
                case -1: return North;
                case 0: assert(false);
                case 1: return South;
            }
        case 1:
            switch (sign(y))
            {
                case -1: return NorthEast;
                case 0: return East;
                case 1: return SouthEast;
            }
    }

    assert(false);
}


template<typename T>
const Vector2Base<T> Vector2Base<T>::zeroVector = Vector2Base<T>(0, 0);

template<typename T>
inline Vector2Base<T> abs(Vector2Base<T> vector)
{
    return Vector2Base<T>(abs(vector.x), abs(vector.y));
}

template<typename T>
inline Vector2Base<T> sign(Vector2Base<T> vector)
{
    return Vector2Base<T>(sign(vector.x), sign(vector.y));
}

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

template<typename T>
inline Vector2Base<T> makeRandomVector(T max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(max), randInt(max) };
}

template<typename T>
inline Vector2Base<T> makeRandomVector(T min, T max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(min, max), randInt(min, max) };
}

namespace std
{
    template<>
    struct hash<Vector2>
    {
        size_t operator()(Vector2 vector) const
        {
            return (vector.x * 73856093) ^ (vector.y * 19349663);
        }
    };
}

/// 3D vector structure for representing values with x, y, and z components.
template<typename T>
struct Vector3Base
{
    T x, y, z;

    Vector3Base() = default;
    constexpr Vector3Base(T x, T y, T z) : x(x), y(y), z(z) {}
    explicit constexpr Vector3Base(Vector2Base<T> vector) : x(vector.x), y(vector.y), z(0) {}

    Vector3Base& operator+=(Vector3Base vector) { x += vector.x; y += vector.y; z += vector.z; return *this; }
    Vector3Base& operator-=(Vector3Base vector) { x -= vector.x; y -= vector.y; z -= vector.z; return *this; }
    Vector3Base& operator*=(Vector3Base vector) { x *= vector.x; y *= vector.y; z *= vector.z; return *this; }
    Vector3Base& operator/=(Vector3Base vector) { x /= vector.x; y /= vector.y; z /= vector.z; return *this; }
    Vector3Base& operator%=(Vector3Base vector) { x %= vector.x; y %= vector.y; z %= vector.z; return *this; }

    Vector3Base operator+(Vector3Base vector) const { return Vector3Base(x + vector.x, y + vector.y, z + vector.z); }
    Vector3Base operator-(Vector3Base vector) const { return Vector3Base(x - vector.x, y - vector.y, z - vector.z); }
    Vector3Base operator*(Vector3Base vector) const { return Vector3Base(x * vector.x, y * vector.y, z * vector.z); }
    Vector3Base operator/(Vector3Base vector) const { return Vector3Base(x / vector.x, y / vector.y, z / vector.z); }
    Vector3Base operator%(Vector3Base vector) const { return Vector3Base(x % vector.x, y % vector.y, z % vector.z); }

    Vector3Base operator+() const { return *this; }
    Vector3Base operator-() const { return Vector3Base(-x, -y, -z); }

    bool operator==(Vector3Base vector) const { return x == vector.x && y == vector.y && z == vector.z; }
    bool operator!=(Vector3Base vector) const { return x != vector.x || y != vector.y || z != vector.z; }

    Vector3Base divideRoundingDown(int divisor) const
    {
        return Vector3Base(::divideRoundingDown(x, divisor),
                           ::divideRoundingDown(y, divisor),
                           ::divideRoundingDown(z, divisor));
    }
};

namespace std
{
    template<>
    struct hash<Vector3>
    {
        size_t operator()(Vector3 vector) const
        {
            return (vector.x * 73856093) ^ (vector.y * 19349663) ^ (vector.z * 83492791);
        }
    };
}

struct Rect
{
    Vector2 position; ///< Top-left corner
    Vector2 size;

    Rect() = default;
    constexpr Rect(Vector2 position, Vector2 size) : position(position), size(size) {}
    constexpr Rect(int x, int y, int w, int h) : position(x, y), size(w, h) {}

    int getLeft() const { return position.x; }
    int getRight() const { assert(size.x > 0); return position.x + size.x - 1; }
    int getTop() const { return position.y; }
    int getBottom() const { assert(size.y > 0); return position.y + size.y - 1; }
    Vector2 getCenter() const { return position + size / 2; }
    int getWidth() const { return size.x; }
    int getHeight() const { return size.y; }

    int getArea() const { return size.getArea(); }
    int getPerimeter() const { return 2 * (size.x + size.y); }
    bool isSquare() const { return size.x == size.y; }

    Rect offset(Vector2 offset) const { return Rect(position + offset, size); }
    Rect inset(Vector2 amount) const { return Rect(position + amount, size - amount * 2); }

    bool intersects(Rect other) const
    {
        return getLeft() < other.getRight() && getRight() > other.getLeft()
            && getTop() < other.getBottom() && getBottom() > other.getTop();
    }
};

template<typename T>
inline bool Vector2Base<T>::isWithin(Rect rect) const
{
    return x >= rect.getLeft() && x <= rect.getRight() &&
           y >= rect.getTop() && y <= rect.getBottom();
}

inline Vector2 makeRandomVectorInside(Rect rect)
{
    return Vector2(randInt(rect.getLeft(), rect.getRight()),
                   randInt(rect.getTop(), rect.getBottom()));
}
