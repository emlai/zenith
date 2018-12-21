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
}

Dir8 randomDir8()
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

    assert(false);
}

using Vector2 = Vector2Base<int>;
using Vector2f = Vector2Base<double>;
using Vector3 = Vector3Base<int>;
using Vector3f = Vector3Base<double>;

extern const Vector2 directionVectors[]; // Definition in math.cpp.

/// 2D vector structure for representing values with x and y components.
struct Vector2Base<T>
{
    T x, y;

    Vector2Base() {}
    constexpr Vector2Base(T x, T y) : x(x), y(y) {}
    constexpr Vector2Base(Dir8 direction) : Vector2Base(directionVectors[direction]) {}
    template<typename U>
    explicit constexpr Vector2Base(Vector2Base<U> vector) : x(T(vector.x)), y(T(vector.y)) {}
    explicit constexpr Vector2Base(Vector3Base<T> vector) : x(vector.x), y(vector.y) {}

    Vector2Base operator+=(Vector2Base vector) { x += vector.x; y += vector.y; return this; }
    Vector2Base operator-=(Vector2Base vector) { x -= vector.x; y -= vector.y; return this; }
    Vector2Base operator=(Vector2Base vector) { x *= vector.x; y *= vector.y; return this; }
    Vector2Base operator/=(Vector2Base vector) { x /= vector.x; y /= vector.y; return this; }
    Vector2Base operator%=(Vector2Base vector) { x %= vector.x; y %= vector.y; return this; }

    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator=(U multiplier) { x *= multiplier; y *= multiplier; return this; }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator/=(U divisor) { x /= divisor; y /= divisor; return this; }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator%=(U divisor) { x %= divisor; y %= divisor; return this; }

    Vector2Base operator+(Vector2Base vector) { return Vector2Base(x + vector.x, y + vector.y); }
    Vector2Base operator-(Vector2Base vector) { return Vector2Base(x - vector.x, y - vector.y); }
    Vector2Base operator*(Vector2Base vector) { return Vector2Base(x * vector.x, y * vector.y); }
    Vector2Base operator/(Vector2Base vector) { return Vector2Base(x / vector.x, y / vector.y); }
    Vector2Base operator%(Vector2Base vector) { return Vector2Base(x % vector.x, y % vector.y); }

    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator*(U multiplier) { return Vector2Base(T(x * multiplier), T(y * multiplier)); }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator/(U divisor) { return Vector2Base(T(x / divisor), T(y / divisor)); }
    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Vector2Base operator%(U divisor) { return Vector2Base(T(x % divisor), T(y % divisor)); }

    Vector2Base operator+() { return this; }
    Vector2Base operator-() { return Vector2Base(-x, -y); }

    bool operator==(Vector2Base vector) { return x == vector.x && y == vector.y; }
    bool operator!=(Vector2Base vector) { return x != vector.x || y != vector.y; }

    var getLength() { return std::sqrt(getLengthSquared()); }
    var getLengthSquared() { return x * x + y * y; }
    var getArea() { return x * y; }
    bool isZero() { return x == 0 && y == 0; }
    template<typename U>
    bool isWithin(Vector2Base<U>);
    bool isWithin(struct Rect);

    Vector2 divideRoundingDown(int divisor)
    {
        return Vector2(::divideRoundingDown(x, divisor), ::divideRoundingDown(y, divisor));
    }

    Vector2 divideRoundingDown(Vector2 divisor)
    {
        return Vector2(::divideRoundingDown(x, divisor.x), ::divideRoundingDown(y, divisor.y));
    }

    Dir8 getDir8();

    const Vector2Base zeroVector;
}

template<typename T>
template<typename U>
bool Vector2Base<T>::isWithin(Vector2Base<U> vector)
{
    return x >= 0 && U(x) < vector.x && y >= 0 && U(y) < vector.y;
}

template<typename T>
Dir8 Vector2Base<T>::getDir8()
{
    if (isZero()) return NoDir;
    double angle = std::atan2(y, x);
    int octant = static_cast<int>(std::round(8 * angle / (2 * pi) + 8)) % 8;
    return static_cast<Dir8>(octant + 1);
}

template<typename T>
const Vector2Base<T> Vector2Base<T>::zeroVector = Vector2Base<T>(0, 0);

template<typename T>
Vector2Base<T> abs(Vector2Base<T> vector)
{
    return Vector2Base<T>(abs(vector.x), abs(vector.y));
}

template<typename T>
Vector2Base<T> sign(Vector2Base<T> vector)
{
    return Vector2Base<T>(sign(vector.x), sign(vector.y));
}

template<typename T>
var getDistanceSquared(Vector2Base<T> a, Vector2Base<T> b)
{
    return (b - a).getLengthSquared();
}

template<typename T>
var getDistance(Vector2Base<T> a, Vector2Base<T> b)
{
    return std::sqrt(getDistanceSquared(a, b));
}

template<typename T>
Vector2Base<T> makeRandomVector(Vector2Base<T> max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(max.x), randInt(max.y) }
}

template<typename T>
Vector2Base<T> makeRandomVector(Vector2Base<T> min, Vector2Base<T> max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(min.x, max.x), randInt(min.y, max.y) }
}

template<typename T>
Vector2Base<T> makeRandomVector(T max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(max), randInt(max) }
}

template<typename T>
Vector2Base<T> makeRandomVector(T min, T max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(min, max), randInt(min, max) }
}

namespace std
{
    template<>
    struct hash<Vector2>
    {
        size_t operator()(Vector2 vector)
        {
            return (vector.x * 73856093) ^ (vector.y * 19349663);
        }
    }
}

/// 3D vector structure for representing values with x, y, and z components.
struct Vector3Base<T>
{
    T x, y, z;

    Vector3Base() {}
    constexpr Vector3Base(T x, T y, T z) : x(x), y(y), z(z) {}
    explicit constexpr Vector3Base(Vector2Base<T> vector) : x(vector.x), y(vector.y), z(0) {}

    Vector3Base operator+=(Vector3Base vector) { x += vector.x; y += vector.y; z += vector.z; return this; }
    Vector3Base operator-=(Vector3Base vector) { x -= vector.x; y -= vector.y; z -= vector.z; return this; }
    Vector3Base operator=(Vector3Base vector) { x *= vector.x; y *= vector.y; z *= vector.z; return this; }
    Vector3Base operator/=(Vector3Base vector) { x /= vector.x; y /= vector.y; z /= vector.z; return this; }
    Vector3Base operator%=(Vector3Base vector) { x %= vector.x; y %= vector.y; z %= vector.z; return this; }

    Vector3Base operator+(Vector3Base vector) { return Vector3Base(x + vector.x, y + vector.y, z + vector.z); }
    Vector3Base operator-(Vector3Base vector) { return Vector3Base(x - vector.x, y - vector.y, z - vector.z); }
    Vector3Base operator*(Vector3Base vector) { return Vector3Base(x * vector.x, y * vector.y, z * vector.z); }
    Vector3Base operator/(Vector3Base vector) { return Vector3Base(x / vector.x, y / vector.y, z / vector.z); }
    Vector3Base operator%(Vector3Base vector) { return Vector3Base(x % vector.x, y % vector.y, z % vector.z); }

    Vector3Base operator+() { return this; }
    Vector3Base operator-() { return Vector3Base(-x, -y, -z); }

    bool operator==(Vector3Base vector) { return x == vector.x && y == vector.y && z == vector.z; }
    bool operator!=(Vector3Base vector) { return x != vector.x || y != vector.y || z != vector.z; }

    Vector3Base divideRoundingDown(int divisor)
    {
        return Vector3Base(::divideRoundingDown(x, divisor),
                           ::divideRoundingDown(y, divisor),
                           ::divideRoundingDown(z, divisor));
    }
}

namespace boost
{
    template<>
    struct hash<Vector3>
    {
        size_t operator()(Vector3 vector)
        {
            return (size_t(vector.x) * 73856093) ^ (size_t(vector.y) * 19349663) ^ (size_t(vector.z) * 83492791);
        }
    }
}

struct Rect
{
    Vector2 position; ///< Top-left corner
    Vector2 size;

    Rect() {}
    constexpr Rect(Vector2 position, Vector2 size) : position(position), size(size) {}
    constexpr Rect(int x, int y, int w, int h) : position(x, y), size(w, h) {}

    int getLeft() { return position.x; }
    int getRight() { assert(size.x > 0); return position.x + size.x - 1; }
    int getTop() { return position.y; }
    int getBottom() { assert(size.y > 0); return position.y + size.y - 1; }
    Vector2 getCenter() { return position + size / 2; }
    int getWidth() { return size.x; }
    int getHeight() { return size.y; }

    int getArea() { return size.getArea(); }
    int getPerimeter() { return 2 * (size.x + size.y); }
    bool isSquare() { return size.x == size.y; }

    Rect offset(Vector2 offset) { return Rect(position + offset, size); }
    Rect inset(Vector2 amount) { return Rect(position + amount, size - amount * 2); }

    bool intersects(Rect other)
    {
        return getLeft() < other.getRight() && getRight() > other.getLeft()
            && getTop() < other.getBottom() && getBottom() > other.getTop();
    }
}

template<typename T>
bool Vector2Base<T>::isWithin(Rect rect)
{
    return x >= rect.getLeft() && x <= rect.getRight() &&
           y >= rect.getTop() && y <= rect.getBottom();
}

Vector2 makeRandomVectorInside(Rect rect)
{
    return Vector2(randInt(rect.getLeft(), rect.getRight()),
                   randInt(rect.getTop(), rect.getBottom()));
}
