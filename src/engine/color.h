#pragma once

#include "math.h"
#include <cstdint>

struct Color
{
    uint8_t r, g, b, a;

    Color() = default;

    explicit Color(uint32_t intValue)
    {
        r = (intValue >> 24) & 0xff;
        g = (intValue >> 16) & 0xff;
        b = (intValue >> 8) & 0xff;
        a = intValue & 0xff;
    }

    Color(int red, int green, int blue, int alpha = 0xff)
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    uint32_t intValue() const
    {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }

    explicit operator bool() const
    {
        return intValue() != 0;
    }

    double getLuminance() const
    {
        return (0.299 * r + 0.587 * g + 0.114 * b) / 0xff;
    }

    void lighten(Color color)
    {
        r = std::max(r, color.r);
        g = std::max(g, color.g);
        b = std::max(b, color.b);
    }

    Color operator*(double mod) const
    {
        auto temperatureCoefficient = 0.25 * 0xff;
        int delta = int(sign(mod - 1.0) * ((2.0 - mod) * mod - 1.0) * temperatureCoefficient);
        int red = limit(int(r * mod + 0.5 - delta), 0, 0xff);
        int green = limit(int(g * mod + 0.5), 0, 0xff);
        int blue = limit(int(b * mod + 0.5 + delta), 0, 0xff);
        return Color(red, green, blue, a);
    }

    static const Color white;
    static const Color black;
    static const Color none;
};
