#include "filesystem.h"
#include <fstream>

bool fs::exists(const char* path)
{
    std::ifstream file(path);
    return file.good();
}
