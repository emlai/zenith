#include "geometry.h"
#include "math.h"
#include <cmath>
#include <string>

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

    ASSERT(false);
}

int divFloor(int dividend, int divisor)
{
    const int quotient = dividend / divisor;

    if ((dividend % divisor != 0) && ((dividend < 0) != (divisor < 0)))
        return quotient - 1;
    else
        return quotient;
}

// TODO: Move to header and make inline once Visual Studio gains inline variable support.
const Vector2 Vector2::zero(0, 0);

bool Vector2::isWithin(Rect rect) const
{
    return x >= rect.getLeft() && x <= rect.getRight() &&
           y >= rect.getTop() && y <= rect.getBottom();
}

Vector2 Vector2::divFloor(int divisor) const
{
    return Vector2(::divFloor(x, divisor), ::divFloor(y, divisor));
}

Vector2 Vector2::divFloor(Vector2 divisor) const
{
    return Vector2(::divFloor(x, divisor.x), ::divFloor(y, divisor.y));
}

Dir8 Vector2::getDir8() const
{
    if (isZero()) return NoDir;
    double angle = std::atan2(y, x);
    int octant = static_cast<int>(std::round(8 * angle / (2 * M_PI) + 8)) % 8;
    return static_cast<Dir8>(octant + 1);
}

std::string Vector2::toString() const
{
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

double Vector2::getLength() const
{
    return std::sqrt(getLengthSquared());
}

Vector2 makeRandomVectorInside(Rect rect)
{
    return Vector2(randInt(rect.getLeft(), rect.getRight()),
                   randInt(rect.getTop(), rect.getBottom()));
}

Vector2 abs(Vector2 vector)
{
    return Vector2(abs(vector.x), abs(vector.y));
}

Vector2 sign(Vector2 vector)
{
    return Vector2(sign(vector.x), sign(vector.y));
}

int getDistanceSquared(Vector2 a, Vector2 b)
{
    return (b - a).getLengthSquared();
}

double getDistance(Vector2 a, Vector2 b)
{
    return std::sqrt(getDistanceSquared(a, b));
}

Vector2 makeRandomVector(Vector2 max)
{
    return { randInt(max.x), randInt(max.y) };
}

Vector2 makeRandomVector(Vector2 min, Vector2 max)
{
    return { randInt(min.x, max.x), randInt(min.y, max.y) };
}

Vector2 makeRandomVector(int max)
{
    return { randInt(max), randInt(max) };
}

Vector2 makeRandomVector(int min, int max)
{
    return { randInt(min, max), randInt(min, max) };
}

Vector3 Vector3::divFloor(int divisor) const
{
    return Vector3(::divFloor(x, divisor), ::divFloor(y, divisor), ::divFloor(z, divisor));
}

bool Rect::intersects(Rect other) const
{
    return getLeft() < other.getRight() && getRight() > other.getLeft()
        && getTop() < other.getBottom() && getBottom() > other.getTop();
}
