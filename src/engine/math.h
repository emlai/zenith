#pragma once

#include "assert.h"
#include <algorithm>
#include <functional>
#include <limits>
#include <random>

template<typename T>
T sign(T value)
{
    return (value > 0) - (value < 0);
}

using RNG = std::mt19937;
extern RNG rng;

template<typename T> T randInt(T max = std::numeric_limits<T>::max());
template<typename T> T randInt(T min, T max);
template<> RNG::result_type randInt(RNG::result_type max);
template<> RNG::result_type randInt(RNG::result_type min, RNG::result_type max);
double randFloat(double max = 1.0);
double randFloat(double min, double max);
double randNormal(double stdDev = 1.0);

template<typename T>
T randInt(T max)
{
    return T(randInt(RNG::result_type(max)));
}

template<typename T>
T randInt(T min, T max)
{
    return T(randInt(RNG::result_type(min), RNG::result_type(max)));
}

template<typename IndexableContainer>
auto& randomElement(IndexableContainer&& container)
{
    ASSERT(!container.empty());
    return container[randInt(container.size() - 1)];
}
