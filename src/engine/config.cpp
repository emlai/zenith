#include "config.h"
#include <cassert>
#include <cctype>
#include <fstream>

/// Wrapper around std::ifstream that keeps track of the current line and column.
class ConfigReader
{
public:
    ConfigReader(std::string_view filePath);
    int get();
    void unget(int);
    int peek();
    std::string getId();
    std::string getDoubleQuotedString();
    std::runtime_error syntaxError(std::string_view message) const;
    std::runtime_error syntaxError(std::string_view expected, char actual) const;

private:
    std::string filePath;
    std::ifstream file;
    int line;
    int column;
};

ConfigReader::ConfigReader(std::string_view filePath)
:   filePath(filePath), file(this->filePath), line(1), column(0)
{
    if (!file)
        throw std::runtime_error("Couldn't open \"" + filePath + "\"!");
}

int ConfigReader::get()
{
    while (true)
    {
        auto ch = file.get();

        if (ch == '\n')
        {
            ++line;
            column = 0;
        }
        else
        {
            ++column;

            if (!std::isspace(ch))
                return ch;
        }
    }
}

std::string ConfigReader::getId()
{
    auto ch = get();

    if (!std::isalpha(ch))
        throw syntaxError("id", char(ch));

    std::string string(1, char(ch));

    while (true)
    {
        auto ch = file.get();

        if (ch == '\n')
        {
            ++line;
            column = 0;
        }
        else
            ++column;

        if (!std::isalpha(ch))
        {
            file.unget();
            return string;
        }

        string += char(ch);
    }
}

std::string ConfigReader::getDoubleQuotedString()
{
    auto ch = file.get();

    if (ch != '"')
        throw syntaxError("'\"'", char(ch));

    std::string string;

    while (true)
    {
        auto ch = file.get();

        if (ch == '\n')
        {
            ++column;
            throw syntaxError("newline inside double-quoted string");
        }
        else
        {
            ++column;

            if (ch == '"')
                return string;

            string += char(ch);
        }
    }
}

void ConfigReader::unget(int ch)
{
    file.putback(static_cast<char>(ch));

    if (ch == '\n')
        --line;
    else
        --column;
}

int ConfigReader::peek()
{
    if (file.peek() == ' ' || file.peek() == '\n')
        unget(get());

    return file.peek();
}

static std::string charToString(char ch)
{
    switch (ch)
    {
        case '\n': return "newline";
        default: return "'" + std::string(1, ch) + "'";
    }
}

std::runtime_error ConfigReader::syntaxError(std::string_view message) const
{
    return std::runtime_error("Syntax error in " + filePath + " (line " + std::to_string(line) +
                              ", column " + std::to_string(column) + "): " + message);
}

std::runtime_error ConfigReader::syntaxError(std::string_view expected, char actual) const
{
    return syntaxError("expected " + expected + ", got " + charToString(actual));
}

std::vector<std::string> Config::getToplevelKeys() const
{
    std::vector<std::string> keys;

    for (auto& keyAndValue : data)
    {
        if (auto it = keyAndValue.second.getGroup().getOptional("isAbstract"))
            if (it->isBool() && it->getBool())
                continue;

        keys.push_back(keyAndValue.first);
    }

    return keys;
}

Config::Group Config::parseGroup(ConfigReader& reader)
{
    Config::Group map;

    while (reader.peek() != '[' && reader.peek() != EOF)
    {
        auto key = reader.getId();
        map.insert(std::move(key), parseProperty(reader));
    }

    return map;
}

Config::Value Config::parseProperty(ConfigReader& reader)
{
    auto ch = reader.get();

    if (ch != '=')
        throw reader.syntaxError("'='", char(ch));

    return parseValue(reader);
}

Config::Value Config::parseValue(ConfigReader& reader)
{
    if (reader.peek() == '[')
        return parseArray(reader);
    else
        return parseAtomicValue(reader);
}

Config::Value Config::parseArray(ConfigReader& reader)
{
    assert(reader.peek() == '[');
    reader.get();
    std::vector<Value> values;

    if (reader.peek() == ']')
    {
        reader.get();
        return std::move(values);
    }

    while (true)
    {
        values.push_back(parseValue(reader));
        auto ch = reader.get();

        if (ch == ',')
            continue;

        if (ch == ']')
        {
            reader.unget(ch);
            break;
        }

        throw reader.syntaxError("']' or ','", char(ch));
    }

    reader.get();
    return std::move(values);
}

Config::Value Config::parseNumber(ConfigReader& reader)
{
    std::string value;
    bool hasDot = false;
    bool isHex = false;
    int ch;

    while (true)
    {
        ch = reader.get();

        if (ch == '.')
        {
            if (hasDot || isHex)
                break;

            hasDot = true;
        }
        else if (ch == 'x' && value == "0")
        {
            isHex = true;
        }
        else if (isHex)
        {
            if (!std::isxdigit(ch))
                break;
        }
        else
        {
            if (!std::isdigit(ch))
                break;
        }

        value += char(ch);
    }

    reader.unget(ch);

    if (hasDot)
        return std::stod(value);
    else if (isHex)
        return std::stoll(value, nullptr, 16);
    else
        return std::stoll(value);
}

Config::Value Config::parseAtomicValue(ConfigReader& reader)
{
    auto ch = reader.peek();

    if (ch == '"')
        return reader.getDoubleQuotedString();

    if (std::isdigit(ch))
        return parseNumber(reader);

    if (std::isalpha(ch))
    {
        auto id = reader.getId();

        if (id == "true")
            return true;

        if (id == "false")
            return false;

        return Value(std::move(id));
    }

    throw reader.syntaxError("number, id, or double-quoted string", char(ch));
}

Config::Config(std::string_view filePath)
{
    ConfigReader reader(filePath);

    while (true)
    {
        if (reader.peek() == EOF)
            break;

        auto ch = reader.get();

        switch (ch)
        {
            case '[':
            {
                std::string id = reader.getId();
                ch = reader.get();

                if (ch != ']')
                    throw reader.syntaxError("']'", char(ch));

                data.insert(std::move(id), parseGroup(reader));
                break;
            }

            default:
            {
                reader.unget(ch);
                std::string id = reader.getId();
                data.insert(std::move(id), parseProperty(reader));
                break;
            }
        }
    }
}

void Config::printValue(std::ostream& stream, const Config::Value& value) const
{
    switch (value.getType())
    {
        case Value::Type::Bool:
            stream << (value.getBool() ? "true" : "false");
            break;
        case Value::Type::Int:
            stream << value.getInt();
            break;
        case Value::Type::Float:
            stream << value.getFloat();
            break;
        case Value::Type::String:
            stream << value.getString();
            break;
        case Value::Type::List:
        {
            stream << "[";
            auto& values = value.getList();
            for (auto& value : values)
            {
                printValue(stream, value);
                if (&value != &values.back())
                    stream << ", ";
            }
            stream << "]";
            break;
        }
        case Value::Type::Group:
            assert(false && "unimplemented");
            break;
    }
}

void Config::writeToFile(const std::string& filePath) const
{
    std::ofstream file(filePath);

    for (auto& [key, value] : data)
    {
        file << key << " = ";
        printValue(file, value);
        file << '\n';
    }
}

Config::Value::Value(bool value) : boolean(value), type(Type::Bool) {}
Config::Value::Value(Integer value) : integer(value), type(Type::Int) {}
Config::Value::Value(double value) : floatingPoint(value), type(Type::Float) {}
Config::Value::Value(std::string value) : string(std::move(value)), type(Type::String) {}
Config::Value::Value(std::vector<Value> value) : list(std::move(value)), type(Type::List) {}
Config::Value::Value(Group value) : group(std::make_unique<Group>(std::move(value))), type(Type::Group) {}

Config::Value::Value(Value&& value)
{
    type = value.type;

    switch (type)
    {
        case Type::Bool:
            boolean = value.boolean;
            break;
        case Type::Int:
            integer = value.integer;
            break;
        case Type::Float:
            floatingPoint = value.floatingPoint;
            break;
        case Type::String:
            new (&string) auto(std::move(value.string));
            break;
        case Type::List:
            new (&list) auto(std::move(value.list));
            break;
        case Type::Group:
            new (&group) auto(std::move(value.group));
            break;
    }
}

Config::Value::~Value()
{
    switch (type)
    {
        case Type::Bool:
        case Type::Int:
        case Type::Float:
            break;
        case Type::String:
            string.~basic_string();
            break;
        case Type::List:
            list.~vector();
            break;
        case Type::Group:
            group.~GroupPtr();
            break;
    }
}
