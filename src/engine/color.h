#pragma once

#include "math.h"
#include <cstdint>

struct Color
{
    enum Channel { Red, Green, Blue, Alpha };

    static constexpr int channelCount = 4;
    static constexpr int bitsPerChannel = 8;
    static constexpr int max = (1 << bitsPerChannel) - 1;
    static constexpr int bit[channelCount] =
    {
        3 * bitsPerChannel, 2 * bitsPerChannel, 1 * bitsPerChannel, 0 * bitsPerChannel
    };
    static constexpr int temperatureCoefficient = int(0.25 * max);
    static const Color white;
    static const Color black;
    static const Color none;
    static constexpr bool modulateTemperature = true;

    uint32_t value;

    Color() = default;
    explicit Color(uint32_t value) : value(value) {}
    Color(int red, int green, int blue, int alpha = max) : value(createValue(red, green, blue, alpha)) {}

    int get(Channel channel) const { return value >> bit[channel] & max; }
    void set(Channel channel, int n) { value = (~(max << bit[channel]) & value) | n << bit[channel]; }
    void edit(Channel channel, int n) { set(channel, limit(get(channel) + n, 0, max)); }

    int getRed() const { return get(Red); }
    int getGreen() const { return get(Green); }
    int getBlue() const { return get(Blue); }
    int getAlpha() const { return get(Alpha); }

    double getLuminance() const
    {
        return (0.299 * getRed() + 0.587 * getGreen() + 0.114 * getBlue()) / max;
    }

    void lighten(Color other)
    {
        set(Red, std::max(getRed(), other.getRed()));
        set(Green, std::max(getGreen(), other.getGreen()));
        set(Blue, std::max(getBlue(), other.getBlue()));
    }

    Color& operator*=(double mod)
    {
        int delta = int(modulateTemperature * sign(mod - 1.0) * ((2.0 - mod) * mod - 1.0) *
                        temperatureCoefficient);
        int red = int(limit(int(getRed() * mod + 0.5 - delta), 0, int(max)));
        int green = int(limit(int(getGreen() * mod + 0.5), 0, int(max)));
        int blue = int(limit(int(getBlue() * mod + 0.5 + delta), 0, int(max)));
        value = createValue(red, green, blue, getAlpha());
        return *this;
    }

    Color operator*(double mod) const { return Color(*this) *= mod; }

    explicit operator bool() const { return value != 0; }

private:
    static inline uint32_t createValue(int red, int green, int blue, int alpha = max)
    {
        return static_cast<uint32_t>(red << bit[Red] | green << bit[Green] | blue << bit[Blue] | alpha << bit[Alpha]);
    }
};
