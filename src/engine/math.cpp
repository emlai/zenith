#include "math.h"
#include "geometry.h"
#include <random>
#include <algorithm>

const Vector2 directionVectors[] =
{
    {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

RNG rng = RNG(Xorshift1024Star());

void RNG::seed()
{
    std::random_device randomDevice;
    seed((uint64_t(randomDevice()) << 32) | randomDevice());
}

void RNG::seed(RNG::result_type seed)
{
    currentSeed = seed;

    Xorshift64Star seedGenerator{seed};
    std::generate(std::begin(algorithm.state), std::end(algorithm.state), seedGenerator);
}

uint64_t Xorshift1024Star::operator()()
{
    auto s0 = state[index];
    auto s1 = state[index = (index + 1) & 15];
    s1 ^= s1 << 31;
    s1 ^= s1 >> 11;
    s0 ^= s0 >> 30;
    return (state[index] = s0 ^ s1) * 1181783497276652981ULL;
}

uint64_t Xorshift64Star::operator()()
{
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 2685821657736338717ULL;
}

template<>
RNG::result_type randInt(RNG::result_type max)
{
    return std::uniform_int_distribution<RNG::result_type>(0, max)(rng);
}

template<>
RNG::result_type randInt(RNG::result_type min, RNG::result_type max)
{
    return std::uniform_int_distribution<RNG::result_type>(min, max)(rng);
}

double randFloat(double max)
{
    return std::uniform_real_distribution<double>(0, max)(rng);
}

double randFloat(double min, double max)
{
    return std::uniform_real_distribution<double>(min, max)(rng);
}

double randNormal(double stdDev)
{
    return std::normal_distribution<double>(0.0, stdDev)(rng);
}

bool raycastIntegerBresenham(Vector2 source, Vector2 target, const std::function<bool(Vector2)>& process)
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

            if (!process(current))
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

            if (!process(current))
                return false;
        }
    }
    else
    {
        while (current.x != target.x)
        {
            current += sign;

            if (!process(current))
                return false;
        }
    }

    return true;
}
