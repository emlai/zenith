#pragma once

#include <fstream>

namespace fs
{

inline bool exists(const char* path)
{
    std::ifstream file(path);
    return file.good();
}

}
