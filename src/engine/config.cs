class Config
{
public:
    Config() {}
    Config(string filePath);
    template<typename ValueType>
    boost::optional<ValueType> getOptional(string key) const;
    template<typename ValueType>
    ValueType get(string type, string attribute) const;
    template<typename ValueType>
    boost::optional<ValueType> getOptional(string type, string attribute) const;
    List<string> getToplevelKeys() const;
    void set(string key, bool value) { data.insert(key.to_string(), Value(value)); }
    void set(string key, long long value) { data.insert(key.to_string(), Value(value)); }
    void set(string key, double value) { data.insert(key.to_string(), Value(value)); }
    void writeToFile(string filePath) const;

private:
    template<typename Value>
    class Group_
    {
    public:
        Value at(string key) const
        {
            if (var value = getOptional(key))
                return *value;

            throw std::out_of_range(key);
        }

        Value getOptional(string key) const
        {
            var it = properties.find(key);

            if (it != properties.end())
                return it->second;

            return nullptr;
        }

        var begin() const { return properties.begin(); }
        var end() const { return properties.end(); }

        void insert(string key, Value value)
        {
            properties.emplace(std::move(key), std::move(value));
        }

    private:
        Dictionary<string, Value> properties;
    }

    class Value
    {
        using Integer = long long;

    public:
        enum class Type
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
        Value(string value) : string(std::move(value)), type(Type::String) {}
        Value(List<Value> value) : list(std::move(value)), type(Type::List) {}
        Value(Group_<Value> value) : group(std::move(value)), type(Type::Group) {}
        Value(Value value);
        ~Value();
        Type getType() const { return type; }
        bool isBool() const { return type == Type::Bool; }
        bool isInt() const { return type == Type::Int; }
        bool isFloat() const { return type == Type::Float; }
        bool isString() const { return type == Type::String; }
        bool isList() const { return type == Type::List; }
        bool isGroup() const { return type == Type::Group; }
        bool getBool() const { return boolean; }
        Integer getInt() const { return integer; }
        double getFloat() const { return floatingPoint; }
        string getString() const { return string; }
        const List<Value>& getList() const { return list; }
        const Group_<Value>& getGroup() const { return group; }

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
    struct ConversionTraits;
    template<typename OutputType>
    static boost::optional<OutputType> convert(Value value)
    {
        return ConversionTraits<OutputType>()(value);
    }

    Group parseGroup(ConfigReader reader);
    Value parseProperty(ConfigReader reader);
    Value parseValue(ConfigReader reader);
    Value parseArray(ConfigReader reader);
    Value parseAtomicValue(ConfigReader reader);
    Value parseNumber(ConfigReader reader);
    void printValue(std::ostream stream, Config::Value value) const;

    Group data;
}

template<typename OutputType>
struct Config::ConversionTraits
{
    boost::optional<OutputType> operator()(Value value);
}

template<>
struct Config::ConversionTraits<bool>
{
    boost::optional<bool> operator()(Value value)
    {
        if (value.isBool())
            return value.getBool();

        return boost::none;
    }
}

template<>
struct Config::ConversionTraits<int>
{
    boost::optional<int> operator()(Value value)
    {
        if (value.isInt())
            return boost::numeric_cast<int>(value.getInt());

        return boost::none;
    }
}

template<>
struct Config::ConversionTraits<unsigned>
{
    boost::optional<unsigned> operator()(Value value)
    {
        if (value.isInt())
            return boost::numeric_cast<unsigned>(value.getInt());

        return boost::none;
    }
}

template<>
struct Config::ConversionTraits<unsigned short>
{
    boost::optional<unsigned short> operator()(Value value)
    {
        if (value.isInt())
            return boost::numeric_cast<unsigned short>(value.getInt());

        return boost::none;
    }
}

template<>
struct Config::ConversionTraits<double>
{
    boost::optional<double> operator()(Value value)
    {
        if (value.isFloat())
            return value.getFloat();

        if (value.isInt())
            return boost::numeric_cast<double>(value.getInt());

        return boost::none;
    }
}

template<>
struct Config::ConversionTraits<string>
{
    boost::optional<string> operator()(Value value)
    {
        if (value.isString())
            return value.getString();

        return boost::none;
    }
}

template<typename ElementType>
struct Config::ConversionTraits<List<ElementType>>
{
    boost::optional<List<ElementType>> operator()(Value value)
    {
        if (!value.isList())
            return boost::none;

        List<ElementType> outputData;

        for (var element : value.getList())
            outputData.push_back(*convert<ElementType>(element));

        return outputData;
    }
}

template<typename ValueType>
boost::optional<ValueType> Config::getOptional(string key) const
{
    if (var value = data.getOptional(key.to_string()))
        return convert<ValueType>(*value);

    return boost::none;
}

template<typename ValueType>
ValueType Config::get(string type, string attribute) const
{
    if (var value = getOptional<ValueType>(type, attribute))
        return ValueType(std::move(*value));
    else
        throw std::runtime_error("attribute \"" + attribute + "\" not found for \"" + type + "\"!");
}

template<typename ValueType>
boost::optional<ValueType> Config::getOptional(string type, string attribute) const
{
    string current = type.to_string();
    string key = attribute.to_string();

    while (true)
    {
        var groupValue = data.getOptional(current);

        if (!groupValue)
            return boost::none;

        var group = groupValue->getGroup();

        if (var value = group.getOptional(key))
        {
            if (var converted = convert<ValueType>(*value))
                return *converted;
            else
                throw std::runtime_error("attribute \"" + attribute + "\" of class \"" + current +
                                         "\" has wrong type!");
        }

        var baseType = group.getOptional("BaseType");

        if (!baseType)
            return boost::none;

        if (baseType->isString())
        {
            var baseTypeString = baseType->getString();

            if (data.getOptional(baseTypeString) == nullptr)
                throw std::runtime_error("BaseType \"" + baseTypeString + "\" doesn't exist!");

            current = baseTypeString;
        }
        else
            throw std::runtime_error("BaseType of \"" + current + "\" has wrong type!");
    }
}
/// Wrapper around std::ifstream that keeps track of the current line and column.
class ConfigReader
{
public:
    ConfigReader(string filePath);
    int get();
    void unget(int);
    int peek();
    string getId();
    string getDoubleQuotedString();
    std::runtime_error syntaxError(string message) const;
    std::runtime_error syntaxError(string expected, char actual) const;

private:
    string filePath;
    std::ifstream file;
    int line;
    int column;
}

ConfigReader::ConfigReader(string filePath)
:   filePath(filePath), file(this->filePath), line(1), column(0)
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

static string charToString(char ch)
{
    switch (ch)
    {
        case '\n': return "newline";
        default: return "'" + string(1, ch) + "'";
    }
}

std::runtime_error ConfigReader::syntaxError(string message) const
{
    return std::runtime_error("Syntax error in " + filePath + " (line " + std::to_string(line) +
                              ", column " + std::to_string(column) + "): " + message);
}

std::runtime_error ConfigReader::syntaxError(string expected, char actual) const
{
    return syntaxError("expected " + expected + ", got " + charToString(actual));
}

List<string> Config::getToplevelKeys() const
{
    List<string> keys;

    for (var keyAndValue : data)
    {
        if (var it = keyAndValue.second.getGroup().getOptional("isAbstract"))
            if (it->isBool() && it->getBool())
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
        map.insert(std::move(key), parseProperty(reader));
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
        return std::move(values);
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
    return std::move(values);
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
        return std::stoll(value, nullptr, 16);
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

        return Value(std::move(id));
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
                data.insert(std::move(id), parseGroup(reader));
                break;
            case '=':
                reader.unget(ch);
                data.insert(std::move(id), parseProperty(reader));
                break;
            default:
                throw reader.syntaxError("'{' or '='", char(ch));
        }
    }
}

void Config::printValue(std::ostream stream, Config::Value value) const
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
            for (var value : values)
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

void Config::writeToFile(string filePath) const
{
    std::ofstream file(filePath.to_string());

    for (var keyAndValue : data)
    {
        file << keyAndValue.first << " = ";
        printValue(file, keyAndValue.second);
        file << ";\n";
    }
}

Config::Value::Value(Value value)
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
            new (string) var(std::move(value.string));
            break;
        case Type::List:
            new (list) var(std::move(value.list));
            break;
        case Type::Group:
            new (group) var(std::move(value.group));
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
            group.~Group();
            break;
    }
}
