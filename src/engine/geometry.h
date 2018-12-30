#pragma once

#include "assert.h"
#include "math.h"
#include "utility.h"
#include <cmath>

struct Vector2;
struct Vector3;
struct Rect;

enum Dir8
{
    NoDir,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest,
    North,
    NorthEast
};

inline Dir8 randomDir8()
{
    switch (randInt(7))
    {
        case 0: return East;
        case 1: return SouthEast;
        case 2: return South;
        case 3: return SouthWest;
        case 4: return West;
        case 5: return NorthWest;
        case 6: return North;
        case 7: return NorthEast;
    }

    ASSERT(false);
}

extern const Vector2 directionVectors[]; // Definition in math.cpp.

struct Vector2
{
    int x, y;

    Vector2() = default;
    constexpr Vector2(int x, int y) : x(x), y(y) {}
    constexpr Vector2(Dir8 direction) : Vector2(directionVectors[direction]) {}
    explicit constexpr Vector2(Vector3 vector);

    Vector2& operator+=(Vector2 vector) { x += vector.x; y += vector.y; return *this; }
    Vector2& operator-=(Vector2 vector) { x -= vector.x; y -= vector.y; return *this; }
    Vector2& operator*=(Vector2 vector) { x *= vector.x; y *= vector.y; return *this; }
    Vector2& operator/=(Vector2 vector) { x /= vector.x; y /= vector.y; return *this; }
    Vector2& operator%=(Vector2 vector) { x %= vector.x; y %= vector.y; return *this; }

    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2& operator*=(U multiplier) { x *= multiplier; y *= multiplier; return *this; }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2& operator/=(U divisor) { x /= divisor; y /= divisor; return *this; }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2& operator%=(U divisor) { x %= divisor; y %= divisor; return *this; }

    Vector2 operator+(Vector2 vector) const { return Vector2(x + vector.x, y + vector.y); }
    Vector2 operator-(Vector2 vector) const { return Vector2(x - vector.x, y - vector.y); }
    Vector2 operator*(Vector2 vector) const { return Vector2(x * vector.x, y * vector.y); }
    Vector2 operator/(Vector2 vector) const { return Vector2(x / vector.x, y / vector.y); }
    Vector2 operator%(Vector2 vector) const { return Vector2(x % vector.x, y % vector.y); }

    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2 operator*(U multiplier) const { return Vector2(int(x * multiplier), int(y * multiplier)); }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2 operator/(U divisor) const { return Vector2(int(x / divisor), int(y / divisor)); }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2 operator%(U divisor) const { return Vector2(int(x % divisor), int(y % divisor)); }

    Vector2 operator+() const { return *this; }
    Vector2 operator-() const { return Vector2(-x, -y); }

    bool operator==(Vector2 vector) const { return x == vector.x && y == vector.y; }
    bool operator!=(Vector2 vector) const { return x != vector.x || y != vector.y; }

    double getLength() const { return std::sqrt(getLengthSquared()); }
    int getLengthSquared() const { return x * x + y * y; }
    auto getArea() const { return x * y; }
    bool isZero() const { return x == 0 && y == 0; }
    bool isWithin(Rect) const;

    Vector2 divFloor(int divisor) const
    {
        return Vector2(::divFloor(x, divisor), ::divFloor(y, divisor));
    }

    Vector2 divFloor(Vector2 divisor) const
    {
        return Vector2(::divFloor(x, divisor.x), ::divFloor(y, divisor.y));
    }

    Dir8 getDir8() const
    {
        if (isZero()) return NoDir;
        double angle = std::atan2(y, x);
        int octant = static_cast<int>(std::round(8 * angle / (2 * M_PI) + 8)) % 8;
        return static_cast<Dir8>(octant + 1);
    }

    std::string toString() const
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }

    static const Vector2 zeroVector;
};

inline const Vector2 Vector2::zeroVector = Vector2(0, 0);

inline Vector2 abs(Vector2 vector)
{
    return Vector2(abs(vector.x), abs(vector.y));
}

inline Vector2 sign(Vector2 vector)
{
    return Vector2(sign(vector.x), sign(vector.y));
}

inline int getDistanceSquared(Vector2 a, Vector2 b)
{
    return (b - a).getLengthSquared();
}

inline double getDistance(Vector2 a, Vector2 b)
{
    return std::sqrt(getDistanceSquared(a, b));
}

inline Vector2 makeRandomVector(Vector2 max)
{
    return { randInt(max.x), randInt(max.y) };
}

inline Vector2 makeRandomVector(Vector2 min, Vector2 max)
{
    return { randInt(min.x, max.x), randInt(min.y, max.y) };
}

inline Vector2 makeRandomVector(int max)
{
    return { randInt(max), randInt(max) };
}

inline Vector2 makeRandomVector(int min, int max)
{
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

struct Vector3
{
    int x, y, z;

    Vector3() = default;
    constexpr Vector3(int x, int y, int z) : x(x), y(y), z(z) {}
    explicit constexpr Vector3(Vector2 vector) : x(vector.x), y(vector.y), z(0) {}

    Vector3& operator+=(Vector3 vector) { x += vector.x; y += vector.y; z += vector.z; return *this; }
    Vector3& operator-=(Vector3 vector) { x -= vector.x; y -= vector.y; z -= vector.z; return *this; }
    Vector3& operator*=(Vector3 vector) { x *= vector.x; y *= vector.y; z *= vector.z; return *this; }
    Vector3& operator/=(Vector3 vector) { x /= vector.x; y /= vector.y; z /= vector.z; return *this; }
    Vector3& operator%=(Vector3 vector) { x %= vector.x; y %= vector.y; z %= vector.z; return *this; }

    Vector3 operator+(Vector3 vector) const { return Vector3(x + vector.x, y + vector.y, z + vector.z); }
    Vector3 operator-(Vector3 vector) const { return Vector3(x - vector.x, y - vector.y, z - vector.z); }
    Vector3 operator*(Vector3 vector) const { return Vector3(x * vector.x, y * vector.y, z * vector.z); }
    Vector3 operator/(Vector3 vector) const { return Vector3(x / vector.x, y / vector.y, z / vector.z); }
    Vector3 operator%(Vector3 vector) const { return Vector3(x % vector.x, y % vector.y, z % vector.z); }

    Vector3 operator+() const { return *this; }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }

    bool operator==(Vector3 vector) const { return x == vector.x && y == vector.y && z == vector.z; }
    bool operator!=(Vector3 vector) const { return x != vector.x || y != vector.y || z != vector.z; }

    Vector3 divFloor(int divisor) const
    {
        return Vector3(::divFloor(x, divisor), ::divFloor(y, divisor), ::divFloor(z, divisor));
    }
};

namespace std
{
    template<>
    struct hash<Vector3>
    {
        size_t operator()(Vector3 vector) const
        {
            return (size_t(vector.x) * 73856093) ^ (size_t(vector.y) * 19349663) ^ (size_t(vector.z) * 83492791);
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
    int getRight() const { ASSERT(size.x > 0); return position.x + size.x - 1; }
    int getTop() const { return position.y; }
    int getBottom() const { ASSERT(size.y > 0); return position.y + size.y - 1; }
    Vector2 getCenter() const { return position + size / 2; }
    int getWidth() const { return size.x; }
    int getHeight() const { return size.y; }
    int getArea() const { return size.getArea(); }
    int getPerimeter() const { return 2 * (size.x + size.y); }
    Rect offset(Vector2 offset) const { return Rect(position + offset, size); }
    Rect inset(Vector2 amount) const { return Rect(position + amount, size - amount * 2); }

    bool intersects(Rect other) const
    {
        return getLeft() < other.getRight() && getRight() > other.getLeft()
            && getTop() < other.getBottom() && getBottom() > other.getTop();
    }
};

inline constexpr Vector2::Vector2(Vector3 vector) : x(vector.x), y(vector.y) {}

inline bool Vector2::isWithin(Rect rect) const
{
    return x >= rect.getLeft() && x <= rect.getRight() &&
           y >= rect.getTop() && y <= rect.getBottom();
}

inline Vector2 makeRandomVectorInside(Rect rect)
{
    return Vector2(randInt(rect.getLeft(), rect.getRight()),
                   randInt(rect.getTop(), rect.getBottom()));
}
