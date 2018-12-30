#include "utility.h"
#include "assert.h"
#include <cctype>

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
    ASSERT(endsWith(a, b));
    return a.substr(0, a.size() - b.size());
}

std::string trim(std::string&& str)
{
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);
    return std::move(str);
}

std::string_view trim(std::string_view str)
{
    str.remove_prefix(str.find_first_not_of(' '));
    str.remove_suffix(str.size() - (str.find_last_not_of(' ') + 1));
    return str;
}

std::string join(const std::vector<std::string>& strings, std::string_view delimiter)
{
    std::string result;

    for (auto& str : strings)
    {
        result += str;

        if (&str != &strings.back())
            result += delimiter;
    }

    return result;
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
