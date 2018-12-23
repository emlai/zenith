#pragma once

#include "geometry.h"

/// Calls `handler` with each point on a line from `source` to `target` as determined by Bresenham's
/// line algorithm. Stops processing and returns false if `handler` returns false. Otherwise returns
/// true after processing all points.
template<typename T>
bool raycast(Vector2 source, Vector2 target, bool (*handler)(Vector2, T*), T* data)
{
    const Vector2 delta = target - source;
    const Vector2 sign = ::sign(delta);
    const Vector2 abs = ::abs(delta) * 2; // x2 to avoid rounding errors
    Vector2 current = source;
    int slope = 0;

    if (abs.x > abs.y)
    {
        while (current.x != target.x)
        {
            if (slope > 0)
            {
                current.y += sign.y;
                slope -= abs.x;
            }

            current.x += sign.x;
            slope += abs.y;

            if (!handler(current, data))
                return false;
        }
    }
    else if (abs.x < abs.y)
    {
        while (current.y != target.y)
        {
            if (slope > 0)
            {
                current.x += sign.x;
                slope -= abs.y;
            }

            current.y += sign.y;
            slope += abs.x;

            if (!handler(current, data))
                return false;
        }
    }
    else
    {
        while (current.x != target.x)
        {
            current += sign;

            if (!handler(current, data))
                return false;
        }
    }

    return true;
}
