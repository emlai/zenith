template<typename T>
std::ostream operator<<(std::ostream outputStream, const List<T>& elements)
{
    for (var element : elements)
        outputStream << element;

    return outputStream;
}

string changeFileExtension(string fileName, string newExtension);

/// Performs integer division, rounding towards negative infinity.
int divideRoundingDown(int dividend, int divisor)
{
    const int quotient = dividend / divisor;

    if ((dividend % divisor != 0) && ((dividend < 0) != (divisor < 0)))
        return quotient - 1;
    else
        return quotient;
}

string toOnOffString(bool value);
string toStringAvoidingDecimalPlaces(double value);

string operator+(string a, string b);

string pascalCaseToSentenceCase(string pascalCaseString);

bool isVowel(char);

struct Deferrer<T>
{
    T deferred;

    Deferrer(T deferred) : deferred(deferred) {}
    Deferrer(Deferrer) {}
    ~Deferrer() { deferred(); }
}

struct DeferHelper {}

template<typename T>
Deferrer<T> operator+(DeferHelper, T deferred)
{
    return Deferrer<T>(deferred);
}

#define DEFER var BOOST_PP_CAT(defer, __LINE__) = DeferHelper() + [&]
string changeFileExtension(string fileName, string newExtension)
{
    var dotPosition = fileName.rfind(".");

    if (dotPosition != string::npos)
    {
        string newFileName = fileName;
        newFileName.replace(dotPosition + 1, string::npos, newExtension);
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

string operator+(string a, string b)
{
    string result;
    result.reserve(a.size() + b.size());
    result.append(a.data(), a.size());
    result.append(b.data(), b.size());
    return result;
}

string pascalCaseToSentenceCase(string pascalCaseString)
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
