#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>

template<typename T>
struct Vector2Base;

using Vector2 = Vector2Base<int>;

template<typename T>
T sign(T value)
{
    return (value > 0) - (value < 0);
}

template<typename T>
T limit(T value, T min, T max)
{
    return std::min(std::max(value, min), max);
}

/* Pseudo-random number generation */

struct Xorshift1024Star
{
    uint64_t operator()();
    uint64_t state[16];
    int index = 0;
};

struct Xorshift64Star
{
    uint64_t operator()();
    uint64_t state;
};

class RNG
{
public:
    using Generator = Xorshift1024Star;
    using result_type = uint64_t;

    RNG(Generator algorithm) : algorithm(std::move(algorithm)) {}
    void seed();
    void seed(RNG::result_type);
    result_type operator()() { return algorithm(); }
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
    static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }

private:
    Generator algorithm;
};

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
    assert(!container.empty());
    return container[randInt(container.size() - 1)];
}
