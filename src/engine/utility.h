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
