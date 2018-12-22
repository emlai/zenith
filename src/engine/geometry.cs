using System;

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

struct Vector2
{
    Sign( x;
    int y;

    Vector2(int x, int y)
    {
        this.x = x;
        this.y = y;
    }

    Vector2(Dir8 direction) : this(directionVectors[direction]) {}

    Vector2(Vector3 vector)
    {
        this.x = vector.x;
        this.y = vector.y;
    }

//    Vector2 operator+=(Vector2 a, Vector2 b) { x += new Vector.x; y += new Vector.y; return this; }
//    Vector2 operator-=(Vector2 a, Vector2 b) { x -= new Vector.x; y -= new Vector.y; return this; }
//    Vector2 operator=(Vector2 a, Vector2 b) { x *= new Vector.x; y *= new Vector.y; return this; }
//    Vector2 operator/=(Vector2 a, Vector2 b) { x /= new Vector.x; y /= new Vector.y; return this; }
//    Vector2 operator%=(Vector2 a, Vector2 b) { x %= new Vector.x; y %= new Vector.y; return this; }
//
//    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
//    Vector2 operator=(U multiplier) { x *= multiplier; y *= multiplier; return this; }
//    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
//    Vector2 operator/=(U divisor) { x /= divisor; y /= divisor; return this; }
//    template<typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
//    Vector2 operator%=(U divisor) { x %= divisor; y %= divisor; return this; }

    public static Vector2 operator+(Vector2 a, Vector2 b) { return new Vector2(a.x + b.x, a.y + b.y); }
    public static Vector2 operator-(Vector2 a, Vector2 b) { return new Vector2(a.x - b.x, a.y - b.y); }
    public static Vector2 operator*(Vector2 a, Vector2 b) { return new Vector2(a.x * b.x, a.y * b.y); }
    public static Vector2 operator/(Vector2 a, Vector2 b) { return new Vector2(a.x / b.x, a.y / b.y); }
    public static Vector2 operator%(Vector2 a, Vector2 b) { return new Vector2(a.x % b.x, a.y % b.y); }
    public static Vector2 operator*(Vector2 a, int b) { return new Vector2(a.x * b, a.y * b); }
    public static Vector2 operator/(Vector2 a, int b) { return new Vector2(a.x / b, a.y / b); }
    public static Vector2 operator%(Vector2 a, int b) { return new Vector2(a.x % b, a.y % b); }
    public static Vector2 operator-(Vector2 a) { return new Vector2(-a.x, -a.y); }
    public static bool operator==(Vector2 a, Vector2 b) { return a.x == b.x && a.y == b.y; }
    public static bool operator!=(Vector2 a, Vector2 b) { return a.x != b.x || a.y != b.y; }

    float getLength() { return MathF.Sqrt(getLengthSquared()); }
    int getLengthSquared() { return x * x + y * y; }
    int getArea() { return x * y; }
    bool isZero() { return x == 0 && y == 0; }

    bool isWithin(Vector2 vector)
    {
        return x >= 0 && x < vector.x && y >= 0 && y < vector.y;
    }

    bool isWithin(struct Rect);

    Vector2 divideRoundingDown(int divisor)
    {
        return new Vector2(::divideRoundingDown(x, divisor), ::divideRoundingDown(y, divisor));
    }

    Vector2 divideRoundingDown(Vector2 divisor)
    {
        return new Vector2(::divideRoundingDown(x, divisor.x), ::divideRoundingDown(y, divisor.y));
    }

    Dir8 getDir8()
    {
        if (isZero()) return NoDir;
        double angle = std::atan2(y, x);
        int octant = static_cast<int>(std::round(8 * angle / (2 * pi) + 8)) % 8;
        return (Dir8) octant + 1;
    }

    static readonly Vector2 zero = new Vector2(0, 0);
}

template<typename T>
Vector2 abs(Vector2 vector)
{
    return new Vector2(abs(vector.x), abs(vector.y));
}

template<typename T>
Vector2 Sign(Vector2 vector)
{
    return new Vector2(Sign(vector.x), Sign(vector.y));
}

template<typename T>
var getDistanceSquared(Vector2 a, Vector2 b)
{
    return (b - a).getLengthSquared();
}

template<typename T>
var getDistance(Vector2 a, Vector2 b)
{
    return std::sqrt(getDistanceSquared(a, b));
}

template<typename T>
Vector2 makeRandomVector(Vector2 max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(max.x), randInt(max.y) }
}

template<typename T>
Vector2 makeRandomVector(Vector2 min, Vector2 max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(min.x, max.x), randInt(min.y, max.y) }
}

template<typename T>
Vector2 makeRandomVector(T max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(max), randInt(max) }
}

template<typename T>
Vector2 makeRandomVector(T min, T max)
{
    // TODO: Add support for floating-point numbers.
    return { randInt(min, max), randInt(min, max) }
}

/// 3D vector structure for representing values with x, y, and z components.
struct Vector3
{
    T x, y, z;

    Vector3() {}
    const Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
    explicit const Vector3(Vector2 vector) : x(vector.x), y(vector.y), z(0) {}

    Vector3 operator+=(Vector3 vector) { x += new Vector.x; y += new Vector.y; z += new Vector.z; return this; }
    Vector3 operator-=(Vector3 vector) { x -= new Vector.x; y -= new Vector.y; z -= new Vector.z; return this; }
    Vector3 operator=(Vector3 vector) { x *= new Vector.x; y *= new Vector.y; z *= new Vector.z; return this; }
    Vector3 operator/=(Vector3 vector) { x /= new Vector.x; y /= new Vector.y; z /= new Vector.z; return this; }
    Vector3 operator%=(Vector3 vector) { x %= new Vector.x; y %= new Vector.y; z %= new Vector.z; return this; }

    Vector3 operator+(Vector3 vector) { return new Vector3(x + vector.x, y + vector.y, z + vector.z); }
    Vector3 operator-(Vector3 vector) { return new Vector3(x - vector.x, y - vector.y, z - vector.z); }
    Vector3 operator*(Vector3 vector) { return new Vector3(x * vector.x, y * vector.y, z * vector.z); }
    Vector3 operator/(Vector3 vector) { return new Vector3(x / vector.x, y / vector.y, z / vector.z); }
    Vector3 operator%(Vector3 vector) { return new Vector3(x % vector.x, y % vector.y, z % vector.z); }

    Vector3 operator+() { return this; }
    Vector3 operator-() { return new Vector3(-x, -y, -z); }

    bool operator==(Vector3 vector) { return x == new Vector.x && y == new Vector.y && z == new Vector.z; }
    bool operator!=(Vector3 vector) { return x != new Vector.x || y != new Vector.y || z != new Vector.z; }

    Vector3 divideRoundingDown(int divisor)
    {
        return new Vector3(::divideRoundingDown(x, divisor),
                           ::divideRoundingDown(y, divisor),
                           ::divideRoundingDown(z, divisor));
    }
}

struct Rect
{
    Vector2 position; ///< Top-left corner
    Vector2 size;

    Rect() {}
    const Rect(Vector2 position, Vector2 size) : position(position), size(size) {}
    const Rect(int x, int y, int w, int h) : position(x, y), size(w, h) {}

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
bool Vector2::isWithin(Rect rect)
{
    return x >= rect.getLeft() && x <= rect.getRight() &&
           y >= rect.getTop() && y <= rect.getBottom();
}

Vector2 makeRandomVectorInside(Rect rect)
{
    return new Vector2(randInt(rect.getLeft(), rect.getRight()),
                   randInt(rect.getTop(), rect.getBottom()));
}
