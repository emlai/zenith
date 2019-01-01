#pragma once

#include <cstdint>

struct Color
{
    uint8_t r, g, b, a;

    Color() = default;
    explicit Color(uint32_t intValue);
    Color(int red, int green, int blue, int alpha = 0xff);
    uint32_t intValue() const;
    explicit operator bool() const;
    double getLuminance() const;
    void lighten(Color color);
    Color operator*(double mod) const;

    static const Color white;
    static const Color black;
    static const Color none;
};
