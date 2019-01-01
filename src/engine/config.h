#pragma once

#include <optional>
#include <unordered_map>
#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class ConfigReader;

class Config
{
public:
    Config() {}
    Config(std::string_view filePath);
    template<typename ValueType>
    std::optional<ValueType> getOptional(std::string_view key) const;
    template<typename ValueType>
    ValueType get(std::string_view type, std::string_view attribute) const;
    template<typename ValueType>
    std::optional<ValueType> getOptional(std::string_view type, std::string_view attribute) const;
    std::vector<std::string> getToplevelKeys() const;
    void set(std::string key, bool value) { data.insert(std::move(key), Value(value)); }
    void set(std::string key, long long value) { data.insert(std::move(key), Value(value)); }
    void set(std::string key, double value) { data.insert(std::move(key), Value(value)); }
    void writeToFile(const std::string& filePath) const;

private:
    class Group;

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
        };

        Value(bool value);
        Value(Integer value);
        Value(double value);
        Value(std::string value);
        Value(std::vector<Value> value);
        Value(Group value);
        Value(Value&& value);
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
        const std::string& getString() const { return string; }
        const std::vector<Value>& getList() const { return list; }
        const Group& getGroup() const { return *group; }

    private:
        using GroupPtr = std::unique_ptr<Group>;

        union
        {
            bool boolean;
            Integer integer;
            double floatingPoint;
            std::string string;
            std::vector<Value> list;
            GroupPtr group;
        };

        Type type;
    };

    class Group
    {
    public:
        const Value* getOptional(const std::string& key) const;
        auto begin() const { return properties.begin(); }
        auto end() const { return properties.end(); }
        void insert(std::string&& key, Value&& value);

    private:
        std::unordered_map<std::string, Value> properties;
    };

    template<typename OutputType>
    struct Converter;
    template<typename OutputType>
    static std::optional<OutputType> convert(const Value& value);

    Group parseGroup(ConfigReader& reader);
    Value parseProperty(ConfigReader& reader);
    Value parseValue(ConfigReader& reader);
    Value parseArray(ConfigReader& reader);
    Value parseAtomicValue(ConfigReader& reader);
    Value parseNumber(ConfigReader& reader);
    void printValue(std::ostream& stream, const Config::Value& value) const;

    Group data;
};
