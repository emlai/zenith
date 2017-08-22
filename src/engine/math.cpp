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

    Xorshift64Star seedGenerator({seed});
    auto& rngState = algorithm.target<Xorshift1024Star>()->state;
    std::generate(std::begin(rngState), std::end(rngState), seedGenerator);
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

double readFloat(double min, double max)
{
    return std::uniform_real_distribution<double>(min, max)(rng);
}

double randNormal(double stdDev)
{
    return std::normal_distribution<double>(0.0, stdDev)(rng);
}
