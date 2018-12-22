using System;
using System.Collections.Generic;
using System.IO;
using static System.Char;
using Group = Group_<Value>;

class Config
{
    Group data;

    ValueType? getOptional<ValueType>(string key) where ValueType : struct
    {
        var value = data.getOptional(key);

        if (value)
            return convert<ValueType>(value);

        return null;
    }

    ValueType get<ValueType>(string type, string attribute) where ValueType : struct
    {
        var value = getOptional<ValueType>(type, attribute);

        if (value != null)
            return value.Value;

        throw new Exception("attribute \"" + attribute + "\" not found for \"" + type + "\"!");
    }

    ValueType? getOptional<ValueType>(string type, string attribute) where ValueType : struct
    {
        string current = type;
        string key = attribute;

        while (true)
        {
            var groupValue = data.getOptional(current);

            if (!groupValue)
                return null;

            var group = groupValue.getGroup();
            var value = group.getOptional(key);

            if (value != null)
            {
                var converted = convert<ValueType>(value);

                if (converted)
                    return converted;

                throw new Exception("attribute \"" + attribute + "\" of class \"" + current + "\" has wrong type!");
            }

            var baseType = group.getOptional("BaseType");

            if (!baseType)
                return null;

            if (baseType.isString())
            {
                var baseTypeString = baseType.getString();

                if (data.getOptional(baseTypeString) == null)
                    throw new Exception("BaseType \"" + baseTypeString + "\" doesn't exist!");

                current = baseTypeString;
            }
            else
                throw new Exception("BaseType of \"" + current + "\" has wrong type!");
        }
    }

    void set(string key, bool value) { data.insert(key, Value(value)); }
    void set(string key, long value) { data.insert(key, Value(value)); }
    void set(string key, double value) { data.insert(key, Value(value)); }

    class Group_<Value> where Value : struct
    {
        Dictionary<string, Value> properties;

        Value at(string key)
        {
            var value = getOptional(key);

            if (value != null)
                return value;

            throw new IndexOutOfRangeException(key);
        }

        Value? getOptional(string key)
        {
            Value value;

            if (properties.TryGetValue(key, out value))
                return value;

            return null;
        }

        void insert(string key, Value value)
        {
            properties[key] = value;
        }
    }

    struct Value
    {
        object value;

        Value(bool value) { this.value = value; }
        Value(long value) { this.value = value; }
        Value(double value) { this.value = value; }
        Value(string value) { this.value = value; }
        Value(List<Value> value) { this.value = value; }
        Value(Group_<Value> value) { this.value = value; }
        Type getType() { return value.GetType(); }
        bool isBool() { return value is bool; }
        bool isInt() { return value is long; }
        bool isFloat() { return value is double; }
        bool isString() { return value is string; }
        bool isList() { return value is List<Value>; }
        bool isGroup() { return value is Group_<Value>; }
        bool getBool() { return (bool) value; }
        long getInt() { return (long) value; }
        double getFloat() { return (double) value; }
        string getString() { return (string) value; }
        List<Value> getList() { return (List<Value>) value; }
        Group_<Value> getGroup() { return (Group_<Value>) value; }
    }

    static OutputType? convert<OutputType>(Value value)
    {
        return ConversionTraits<OutputType>()(value);
    }

    List<string> getToplevelKeys()
    {
        var keys = new List<string>();

        foreach (var keyAndValue in data)
        {
            var it = keyAndValue.second.getGroup().getOptional("isAbstract");

            if (it && it.isBool() && it.getBool())
                continue;

            keys.Add(keyAndValue.first);
        }

        return keys;
    }

    Group parseGroup(ConfigReader reader)
    {
        Group map;

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

    Value parseProperty(ConfigReader reader)
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

    Value parseValue(ConfigReader reader)
    {
        if (reader.peek() == '[')
            return parseArray(reader);
        else
            return parseAtomicValue(reader);
    }

    Value parseArray(ConfigReader reader)
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

    Value parseNumber(ConfigReader reader)
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

    Value parseAtomicValue(ConfigReader reader)
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

    Config(string filePath)
    {
        var reader = new ConfigReader(filePath);

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

    void printValue(std::ostream stream, Value value)
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

    void writeToFile(string filePath)
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
struct ConversionTraits
{
    OutputType? operator()(Value value);
}

template<>
struct ConversionTraits<bool>
{
    bool? operator()(Value value)
    {
        if (value.isBool())
            return value.getBool();

        return null;
    }
}

template<>
struct ConversionTraits<int>
{
    int? operator()(Value value)
    {
        if (value.isInt())
            return boost::numeric_cast<int>(value.getInt());

        return null;
    }
}

template<>
struct ConversionTraits<unsigned>
{
    unsigned? operator()(Value value)
    {
        if (value.isInt())
            return boost::numeric_cast<unsigned>(value.getInt());

        return null;
    }
}

template<>
struct ConversionTraits<unsigned short>
{
    unsigned short? operator()(Value value)
    {
        if (value.isInt())
            return boost::numeric_cast<unsigned short>(value.getInt());

        return null;
    }
}

template<>
struct ConversionTraits<double>
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
struct ConversionTraits<string>
{
    string? operator()(Value value)
    {
        if (value.isString())
            return value.getString();

        return null;
    }
}

template<typename ElementType>
struct ConversionTraits<List<ElementType>>
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

/// Wrapper around StreamReader that keeps track of the current line and column.
class ConfigReader : IDisposable
{
    string filePath;
    StreamReader file;
    int line;
    int column;

    public ConfigReader(string filePath)
    {
        this.filePath = filePath;
        this.file = File.OpenText(filePath);
        this.line = 1;
        this.column = 0;
    }

    public void Dispose()
    {
        file.Dispose();
    }

    char get()
    {
        while (true)
        {
            var ch = (char) file.Read();

            if (ch == '\n')
            {
                ++line;
                column = 0;
            }
            else
            {
                ++column;

                if (!IsWhiteSpace(ch))
                    return ch;
            }
        }
    }

    string getId()
    {
        var ch = get();

        if (!IsLetter(ch))
            throw syntaxError("id", ch);

        var id = ch.ToString();

        while (true)
        {
            ch = (char) file.Peek();

            if (ch == '\n')
            {
                ++line;
                column = 0;
            }
            else
                ++column;

            if (!IsLetter(ch))
                return id;

            file.Read();
            id += ch;
        }
    }

    string getDoubleQuotedString()
    {
        var ch = (char) file.Read();

        if (ch != '"')
            throw syntaxError("'\"'", ch);

        string str = "";

        while (true)
        {
            ch = (char) file.Read();

            if (ch == '\n')
            {
                ++column;
                throw syntaxError("newline inside double-quoted string");
            }

            ++column;

            if (ch == '"')
                return str;

            str += ch;
        }
    }

    void unget(int ch)
    {
        file.putback((char) ch);

        if (ch == '\n')
            --line;
        else
            --column;
    }

    public int peek()
    {
        if (file.Peek() == ' ' || file.Peek() == '\n')
            unget(get());

        return file.Peek();
    }

    static string charToString(char ch)
    {
        switch (ch)
        {
            case '\n': return "newline";
            default: return "'" + ch + "'";
        }
    }

    Exception syntaxError(string message)
    {
        return new Exception("Syntax error in " + filePath + " (line " + line + ", column " + column + "): " + message);
    }

    Exception syntaxError(string expected, char actual)
    {
        return syntaxError("expected " + expected + ", got " + charToString(actual));
    }
}
