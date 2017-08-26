#pragma once

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

std::string changeFileExtension(const std::string& fileName, const std::string& newExtension);

/// Performs integer division, rounding towards negative infinity.
inline int divideRoundingDown(int dividend, int divisor)
{
    const int quotient = dividend / divisor;

    if ((dividend % divisor != 0) && ((dividend < 0) != (divisor < 0)))
        return quotient - 1;
    else
        return quotient;
}
