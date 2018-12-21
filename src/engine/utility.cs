template<typename T>
std::ostream operator<<(std::ostream outputStream, const std::vector<T>& elements)
{
    for (var element : elements)
        outputStream << element;

    return outputStream;
}

string changeFileExtension(boost::string_ref fileName, boost::string_ref newExtension);

/// Performs integer division, rounding towards negative infinity.
inline int divideRoundingDown(int dividend, int divisor)
{
    const int quotient = dividend / divisor;

    if ((dividend % divisor != 0) && ((dividend < 0) != (divisor < 0)))
        return quotient - 1;
    else
        return quotient;
}

string toOnOffString(bool value);
string toStringAvoidingDecimalPlaces(double value);

string operator+(boost::string_ref a, boost::string_ref b);

string pascalCaseToSentenceCase(boost::string_ref pascalCaseString);

bool isVowel(char);

template<typename T>
struct Deferrer
{
    T deferred;

    Deferrer(T deferred) : deferred(std::move(deferred)) {}
    Deferrer(Deferrer) = default;
    ~Deferrer() { deferred(); }
}

struct DeferHelper {}

template<typename T>
Deferrer<T> operator+(DeferHelper, T deferred)
{
    return Deferrer<T>(std::move(deferred));
}

#define DEFER var BOOST_PP_CAT(defer, __LINE__) = DeferHelper() + [&]
string changeFileExtension(boost::string_ref fileName, boost::string_ref newExtension)
{
    var dotPosition = fileName.rfind(".");

    if (dotPosition != string::npos)
    {
        string newFileName = fileName.to_string();
        newFileName.replace(dotPosition + 1, string::npos, newExtension.to_string());
        return newFileName;
    }
    else
        return fileName + "." + newExtension;
}

string toOnOffString(bool value)
{
    return value ? "on" : "off";
}

string toStringAvoidingDecimalPlaces(double value)
{
    var string = std::to_string(value);

    while (string.back() == '0' || string.back() == '.')
        string.pop_back();

    return string;
}

string operator+(boost::string_ref a, boost::string_ref b)
{
    string result;
    result.reserve(a.size() + b.size());
    result.append(a.data(), a.size());
    result.append(b.data(), b.size());
    return result;
}

string pascalCaseToSentenceCase(boost::string_ref pascalCaseString)
{
    string name;
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
