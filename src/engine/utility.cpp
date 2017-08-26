#include "utility.h"

std::string changeFileExtension(boost::string_ref fileName, boost::string_ref newExtension)
{
    auto dotPosition = fileName.rfind(".");

    if (dotPosition != std::string::npos)
    {
        std::string newFileName = fileName.to_string();
        newFileName.replace(dotPosition + 1, std::string::npos, newExtension.to_string());
        return newFileName;
    }
    else
        return fileName + "." + newExtension;
}

std::string operator+(boost::string_ref a, boost::string_ref b)
{
    std::string result;
    result.reserve(a.size() + b.size());
    result.append(a.data(), a.size());
    result.append(b.data(), b.size());
    return result;
}
