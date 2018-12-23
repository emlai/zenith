#include "math.h"
#include "geometry.h"
#include <random>
#include <algorithm>

const Vector2 directionVectors[] =
{
    {0, 0}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
};

RNG rng;

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
