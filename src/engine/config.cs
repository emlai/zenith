class Config
{
    Group data;

    ValueType? getOptional<ValueType>(string key)
    {
        if (var value = data.getOptional(key))
        return convert<ValueType>(value);

        return null;
    }

    ValueType get<ValueType>(string type, string attribute)
    {
        var value = getOptional<ValueType>(type, attribute);
        if (value)
            return ValueType(value);
        else
            throw std::runtime_error("attribute \"" + attribute + "\" not found for \"" + type + "\"!");
    }

    ValueType? getOptional<ValueType>(string type, string attribute)
    {
        string current = type;
        string key = attribute;

        while (true)
        {
            var groupValue = data.getOptional(current);

            if (!groupValue)
                return null;

            var group = groupValue.getGroup();

            if (var value = group.getOptional(key))
            {
                if (var converted = convert<ValueType>(value))
                return converted;
                else
                throw std::runtime_error("attribute \"" + attribute + "\" of class \"" + current +
                                         "\" has wrong type!");
            }

            var baseType = group.getOptional("BaseType");

            if (!baseType)
                return null;

            if (baseType.isString())
            {
                var baseTypeString = baseType.getString();

                if (data.getOptional(baseTypeString) == null)
                    throw std::runtime_error("BaseType \"" + baseTypeString + "\" doesn't exist!");

                current = baseTypeString;
            }
            else
                throw std::runtime_error("BaseType of \"" + current + "\" has wrong type!");
        }
    }
    void set(string key, bool value) { data.insert(key, Value(value)); }
    void set(string key, long value) { data.insert(key, Value(value)); }
    void set(string key, double value) { data.insert(key, Value(value)); }

private:
    template<typename Value>
    class Group_
    {
        Value at(string key)
        {
            if (var value = getOptional(key))
                return value;

            throw std::out_of_range(key);
        }

        Value getOptional(string key)
        {
            var it = properties.find(key);

            if (it != properties.end())
                return it.second;

            return null;
        }

        var begin() { return properties.begin(); }
        var end() { return properties.end(); }

        void insert(string key, Value value)
        {
            properties.emplace(key, value);
        }

    private:
        Dictionary<string, Value> properties;
    }

    class Value
    {
        using Integer = long;
        enum Type
        {
            Bool,
            Int,
            Float,
            String,
            List,
            Group
        }

        Value(bool value) : boolean(value), type(Type::Bool) {}
        Value(Integer value) : integer(value), type(Type::Int) {}
        Value(double value) : floatingPoint(value), type(Type::Float) {}
        Value(string value) : string(value), type(Type::String) {}
        Value(List<Value> value) : list(value), type(Type::List) {}
        Value(Group_<Value> value) : group(value), type(Type::Group) {}
        Value(Value value);
        ~Value();
        Type getType() { return type; }
        bool isBool() { return type == Type::Bool; }
        bool isInt() { return type == Type::Int; }
        bool isFloat() { return type == Type::Float; }
        bool isString() { return type == Type::String; }
        bool isList() { return type == Type::List; }
        bool isGroup() { return type == Type::Group; }
        bool getBool() { return boolean; }
        Integer getInt() { return integer; }
        double getFloat() { return floatingPoint; }
        string getString() { return string; }
        List<Value> getList() { return list; }
        Group_<Value> getGroup() { return group; }

    private:
        union
        {
            bool boolean;
            Integer integer;
            double floatingPoint;
            string string;
            List<Value> list;
            Group_<Value> group;
        }

        Type type;
    }

    using Group = Group_<Value>;

    template<typename OutputType>
    template<typename OutputType>
    static OutputType? convert(Value value)
    {
        return ConversionTraits<OutputType>()(value);
    }

    List<string> Config::getToplevelKeys()
    {
        List<string> keys;

        foreach (var keyAndValue in data)
        {
            if (var it = keyAndValue.second.getGroup().getOptional("isAbstract"))
                if (it.isBool() && it.getBool())
                    continue;

            keys.push_back(keyAndValue.first);
        }

        return keys;
    }

    Config::Group Config::parseGroup(ConfigReader reader)
    {
        Config::Group map;

        while (true)
        {
            var ch = reader.get();

            if (ch == '}')
                break;

            reader.unget(ch);
            var key = reader.getId();
            map.insert(key, parseProperty(reader));
        }

        return map;
    }

    Config::Value Config::parseProperty(ConfigReader reader)
    {
        var ch = reader.get();

        if (ch != '=')
            throw reader.syntaxError("'='", char(ch));

        var value = parseValue(reader);

        ch = reader.get();

        if (ch != ';')
            throw reader.syntaxError("';'", char(ch));

        return value;
    }

    Config::Value Config::parseValue(ConfigReader reader)
    {
        if (reader.peek() == '[')
            return parseArray(reader);
        else
            return parseAtomicValue(reader);
    }

    Config::Value Config::parseArray(ConfigReader reader)
    {
        assert(reader.peek() == '[');
        reader.get();
        List<Value> values;

        if (reader.peek() == ']')
        {
            reader.get();
            return values;
        }

        while (true)
        {
            values.push_back(parseValue(reader));
            var ch = reader.get();

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
        return values;
    }

    Config::Value Config::parseNumber(ConfigReader reader)
    {
        string value;
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
            return std::stoll(value, null, 16);
        else
            return std::stoll(value);
    }

    Config::Value Config::parseAtomicValue(ConfigReader reader)
    {
        var ch = reader.peek();

        if (ch == '"')
            return reader.getDoubleQuotedString();

        if (std::isdigit(ch))
            return parseNumber(reader);

        if (std::isalpha(ch))
        {
            var id = reader.getId();

            if (id == "true")
                return true;

            if (id == "false")
                return false;

            return Value(id);
        }

        throw reader.syntaxError("number, id, or double-quoted string", char(ch));
    }

    Config::Config(string filePath)
    {
        ConfigReader reader(filePath);

        while (true)
        {
            if (reader.peek() == EOF)
                break;

            string id = reader.getId();
            var ch = reader.get();

            switch (ch)
            {
                case '{':
                    data.insert(id, parseGroup(reader));
                    break;
                case '=':
                    reader.unget(ch);
                    data.insert(id, parseProperty(reader));
                    break;
                default:
                    throw reader.syntaxError("'{' or '='", char(ch));
            }
        }
    }

    void Config::printValue(std::ostream stream, Config::Value value)
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
                var values = value.getList();
                foreach (var value in values)
                {
                    printValue(stream, value);
                    if (value != values.back())
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

    void Config::writeToFile(string filePath)
    {
        std::ofstream file(filePath);

        foreach (var keyAndValue in data)
        {
            file << keyAndValue.first << " = ";
            printValue(file, keyAndValue.second);
            file << ";\n";
        }
    }
}

template<typename OutputType>
struct Config::ConversionTraits
{
    OutputType? operator()(Value value);
}

template<>
struct Config::ConversionTraits<bool>
{
    bool? operator()(Value value)
    {
        if (value.isBool())
            return value.getBool();

        return null;
    }
}

template<>
struct Config::ConversionTraits<int>
{
    int? operator()(Value value)
    {
        if (value.isInt())
            return boost::numeric_cast<int>(value.getInt());

        return null;
    }
}

template<>
struct Config::ConversionTraits<unsigned>
{
    unsigned? operator()(Value value)
    {
        if (value.isInt())
            return boost::numeric_cast<unsigned>(value.getInt());

        return null;
    }
}

template<>
struct Config::ConversionTraits<unsigned short>
{
    unsigned short? operator()(Value value)
    {
        if (value.isInt())
            return boost::numeric_cast<unsigned short>(value.getInt());

        return null;
    }
}

template<>
struct Config::ConversionTraits<double>
{
    double? operator()(Value value)
    {
        if (value.isFloat())
            return value.getFloat();

        if (value.isInt())
            return boost::numeric_cast<double>(value.getInt());

        return null;
    }
}

template<>
struct Config::ConversionTraits<string>
{
    string? operator()(Value value)
    {
        if (value.isString())
            return value.getString();

        return null;
    }
}

template<typename ElementType>
struct Config::ConversionTraits<List<ElementType>>
{
    List<ElementType?> operator()(Value value)
    {
        if (!value.isList())
            return null;

        List<ElementType> outputData;

        foreach (var element in value.getList())
            outputData.push_back(convert<ElementType>(element));

        return outputData;
    }
}

/// Wrapper around std::ifstream that keeps track of the current line and column.
class ConfigReader
{
    ConfigReader(string filePath);
    int get();
    void unget(int);
    int peek();
    string getId();
    string getDoubleQuotedString();
    std::runtime_error syntaxError(string message);
    std::runtime_error syntaxError(string expected, char actual);

private:
    string filePath;
    std::ifstream file;
    int line;
    int column;
}

ConfigReader::ConfigReader(string filePath)
:   filePath(filePath), file(this.filePath), line(1), column(0)
{
    if (!file)
        throw std::runtime_error("Couldn't open \"" + filePath + "\"!");
}

int ConfigReader::get()
{
    while (true)
    {
        var ch = file.get();

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

string ConfigReader::getId()
{
    var ch = get();

    if (!std::isalpha(ch))
        throw syntaxError("id", char(ch));

    string string(1, char(ch));

    while (true)
    {
        var ch = file.get();

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

string ConfigReader::getDoubleQuotedString()
{
    var ch = file.get();

    if (ch != '"')
        throw syntaxError("'\"'", char(ch));

    string string;

    while (true)
    {
        var ch = file.get();

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
    file.putback((char) ch);

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

static string charToString(char ch)
{
    switch (ch)
    {
        case '\n': return "newline";
        default: return "'" + string(1, ch) + "'";
    }
}

std::runtime_error ConfigReader::syntaxError(string message)
{
    return std::runtime_error("Syntax error in " + filePath + " (line " + std::to_string(line) +
                              ", column " + std::to_string(column) + "): " + message);
}

std::runtime_error ConfigReader::syntaxError(string expected, char actual)
{
    return syntaxError("expected " + expected + ", got " + charToString(actual));
}

