#include "utility.h"

std::string changeFileExtension(const std::string& fileName, const std::string& newExtension)
{
    auto dotPosition = fileName.rfind(".");

    if (dotPosition != std::string::npos)
    {
        std::string newFileName = fileName;
        newFileName.replace(dotPosition + 1, std::string::npos, newExtension);
        return newFileName;
    }
    else
        return fileName + "." + newExtension;
}
