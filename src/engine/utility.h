#pragma once

#include <string_view>
#include <ostream>
#include <string>
#include <vector>

template<typename T>
std::ostream& operator<<(std::ostream& outputStream, const std::vector<T>& elements)
{
    for (const auto& element : elements)
        outputStream << element;

    return outputStream;
}

std::string changeFileExtension(std::string_view fileName, std::string_view newExtension);

/// Performs integer division, rounding towards negative infinity.
inline int divideRoundingDown(int dividend, int divisor)
{
    const int quotient = dividend / divisor;

    if ((dividend % divisor != 0) && ((dividend < 0) != (divisor < 0)))
        return quotient - 1;
    else
        return quotient;
}

std::string toOnOffString(bool value);
std::string toStringAvoidingDecimalPlaces(double value);

std::string operator+(std::string_view a, std::string_view b);
bool startsWith(std::string_view, std::string_view);
bool endsWith(std::string_view, std::string_view);
std::string_view removeSuffix(std::string_view, std::string_view);

std::string pascalCaseToSentenceCase(std::string_view pascalCaseString);

bool isVowel(char);

template<typename T>
struct IteratorRange
{
    T _begin;
    T _end;

    IteratorRange(T begin, T end) : _begin(begin), _end(end) {}
    T begin() const { return _begin; }
    T end() const { return _end; }
};

template<typename T>
IteratorRange<T> makeIteratorRange(T begin, T end)
{
    return IteratorRange<T>(begin, end);
}

template<typename T>
auto reverse(const T& v)
{
    return makeIteratorRange(v.rbegin(), v.rend());
}

template<typename T>
struct Deferrer
{
    T deferred;

    Deferrer(T deferred) : deferred(std::move(deferred)) {}
    Deferrer(Deferrer&&) = default;
    ~Deferrer() { deferred(); }
};

struct DeferHelper {};

template<typename T>
Deferrer<T> operator+(DeferHelper, T deferred)
{
    return Deferrer<T>(std::move(deferred));
}

#define PP_CAT_I(a, b) a ## b
#define PP_CAT(a, b) PP_CAT_I(a, b)
#define DEFER auto PP_CAT(defer, __LINE__) = DeferHelper() + [&]
