#include "color.h"
#include "math.h"
#include <algorithm>

const Color Color::white(0xff, 0xff, 0xff);
const Color Color::black(0, 0, 0);
const Color Color::none(0);

Color::Color(uint32_t intValue)
{
    r = (intValue >> 24) & 0xff;
    g = (intValue >> 16) & 0xff;
    b = (intValue >> 8) & 0xff;
    a = intValue & 0xff;
}

Color::Color(int red, int green, int blue, int alpha)
{
    r = red;
    g = green;
    b = blue;
    a = alpha;
}

uint32_t Color::intValue() const
{
    return (r << 24) | (g << 16) | (b << 8) | a;
}

Color::operator bool() const
{
    return intValue() != 0;
}

double Color::getLuminance() const
{
    return (0.299 * r + 0.587 * g + 0.114 * b) / 0xff;
}

void Color::lighten(Color color)
{
    r = std::max(r, color.r);
    g = std::max(g, color.g);
    b = std::max(b, color.b);
}

Color Color::operator*(double mod) const
{
    auto temperatureCoefficient = 0.25 * 0xff;
    int delta = int(sign(mod - 1.0) * ((2.0 - mod) * mod - 1.0) * temperatureCoefficient);
    int red = std::clamp(int(r * mod + 0.5 - delta), 0, 0xff);
    int green = std::clamp(int(g * mod + 0.5), 0, 0xff);
    int blue = std::clamp(int(b * mod + 0.5 + delta), 0, 0xff);
    return Color(red, green, blue, a);
}
