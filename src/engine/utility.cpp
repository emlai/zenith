#include "utility.h"
#include <cassert>
#include <cctype>

std::string changeFileExtension(std::string_view fileName, std::string_view newExtension)
{
    auto dotPosition = fileName.rfind(".");

    if (dotPosition != std::string::npos)
    {
        std::string newFileName(fileName);
        newFileName.replace(dotPosition + 1, std::string::npos, newExtension);
        return newFileName;
    }
    else
        return fileName + "." + newExtension;
}

std::string toOnOffString(bool value)
{
    return value ? "on" : "off";
}

std::string toStringAvoidingDecimalPlaces(double value)
{
    auto string = std::to_string(value);

    while (string.back() == '0' || string.back() == '.')
        string.pop_back();

    return string;
}

std::string operator+(std::string_view a, std::string_view b)
{
    std::string result;
    result.reserve(a.size() + b.size());
    result.append(a.data(), a.size());
    result.append(b.data(), b.size());
    return result;
}

bool startsWith(std::string_view a, std::string_view b)
{
    return a.size() >= b.size() && a.substr(0, b.size()) == b;
}

bool endsWith(std::string_view a, std::string_view b)
{
    return a.size() >= b.size() && a.substr(a.size() - b.size()) == b;
}

std::string_view removeSuffix(std::string_view a, std::string_view b)
{
    assert(endsWith(a, b));
    return a.substr(a.size() - b.size());
}

std::string pascalCaseToSentenceCase(std::string_view pascalCaseString)
{
    std::string name;
    name.reserve(pascalCaseString.size());

    for (char ch : pascalCaseString)
    {
        if (std::isupper(ch))
        {
            if (!name.empty())
                name += ' ';

            name += char(std::tolower(ch));
        }
        else
            name += ch;
    }

    return name;
}

bool isVowel(char ch)
{
    return ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u';
}
