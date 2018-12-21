#include <boost/preprocessor/cat.hpp>
#include <boost/utility/string_ref.hpp>
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

std::string changeFileExtension(boost::string_ref fileName, boost::string_ref newExtension);

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

std::string operator+(boost::string_ref a, boost::string_ref b);

std::string pascalCaseToSentenceCase(boost::string_ref pascalCaseString);

bool isVowel(char);

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

#define DEFER auto BOOST_PP_CAT(defer, __LINE__) = DeferHelper() + [&]
#include "utility.h"
#include <cctype>

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

std::string operator+(boost::string_ref a, boost::string_ref b)
{
    std::string result;
    result.reserve(a.size() + b.size());
    result.append(a.data(), a.size());
    result.append(b.data(), b.size());
    return result;
}

std::string pascalCaseToSentenceCase(boost::string_ref pascalCaseString)
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
