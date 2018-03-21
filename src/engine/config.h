#pragma once

#include "utility.h"
#include <optional>
#include <unordered_map>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
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
    template<typename Value>
    class Group_
    {
    public:
        const Value& at(const std::string& key) const
        {
            if (auto value = getOptional(key))
                return *value;

            throw std::out_of_range(key);
        }

        const Value* getOptional(const std::string& key) const
        {
            auto it = properties.find(key);

            if (it != properties.end())
                return &it->second;

            return nullptr;
        }

        auto begin() const { return properties.begin(); }
        auto end() const { return properties.end(); }

        void insert(std::string&& key, Value&& value)
        {
            properties.emplace(std::move(key), std::move(value));
        }

    private:
        std::unordered_map<std::string, Value> properties;
    };

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

        Value(bool value) : boolean(value), type(Type::Bool) {}
        Value(Integer value) : integer(value), type(Type::Int) {}
        Value(double value) : floatingPoint(value), type(Type::Float) {}
        Value(std::string value) : string(std::move(value)), type(Type::String) {}
        Value(std::vector<Value> value) : list(std::move(value)), type(Type::List) {}
        Value(Group_<Value> value) : group(std::move(value)), type(Type::Group) {}
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
        const Group_<Value>& getGroup() const { return group; }

    private:
        union
        {
            bool boolean;
            Integer integer;
            double floatingPoint;
            std::string string;
            std::vector<Value> list;
            Group_<Value> group;
        };

        Type type;
    };

    using Group = Group_<Value>;

    template<typename OutputType>
    struct ConversionTraits;
    template<typename OutputType>
    static std::optional<OutputType> convert(const Value& value)
    {
        return ConversionTraits<OutputType>()(value);
    }

    Group parseGroup(ConfigReader& reader);
    Value parseProperty(ConfigReader& reader);
    Value parseValue(ConfigReader& reader);
    Value parseArray(ConfigReader& reader);
    Value parseAtomicValue(ConfigReader& reader);
    Value parseNumber(ConfigReader& reader);
    void printValue(std::ostream& stream, const Config::Value& value) const;

    Group data;
};

template<typename OutputType>
struct Config::ConversionTraits
{
    std::optional<OutputType> operator()(const Value& value);
};

template<>
struct Config::ConversionTraits<bool>
{
    std::optional<bool> operator()(const Value& value)
    {
        if (value.isBool())
            return value.getBool();

        return std::nullopt;
    }
};

template<>
struct Config::ConversionTraits<int>
{
    std::optional<int> operator()(const Value& value)
    {
        if (value.isInt())
            return static_cast<int>(value.getInt());

        return std::nullopt;
    }
};

template<>
struct Config::ConversionTraits<unsigned>
{
    std::optional<unsigned> operator()(const Value& value)
    {
        if (value.isInt())
            return static_cast<unsigned>(value.getInt());

        return std::nullopt;
    }
};

template<>
struct Config::ConversionTraits<unsigned short>
{
    std::optional<unsigned short> operator()(const Value& value)
    {
        if (value.isInt())
            return static_cast<unsigned short>(value.getInt());

        return std::nullopt;
    }
};

template<>
struct Config::ConversionTraits<double>
{
    std::optional<double> operator()(const Value& value)
    {
        if (value.isFloat())
            return value.getFloat();

        if (value.isInt())
            return static_cast<double>(value.getInt());

        return std::nullopt;
    }
};

template<>
struct Config::ConversionTraits<std::string>
{
    std::optional<std::string> operator()(const Value& value)
    {
        if (value.isString())
            return value.getString();

        return std::nullopt;
    }
};

template<typename ElementType>
struct Config::ConversionTraits<std::vector<ElementType>>
{
    std::optional<std::vector<ElementType>> operator()(const Value& value)
    {
        if (!value.isList())
            return std::nullopt;

        std::vector<ElementType> outputData;

        for (auto& element : value.getList())
            outputData.push_back(*convert<ElementType>(element));

        return outputData;
    }
};

template<typename ValueType>
std::optional<ValueType> Config::getOptional(std::string_view key) const
{
    if (auto value = data.getOptional(std::string(key)))
        return convert<ValueType>(*value);

    return std::nullopt;
}

template<typename ValueType>
ValueType Config::get(std::string_view type, std::string_view attribute) const
{
    if (auto value = getOptional<ValueType>(type, attribute))
        return ValueType(std::move(*value));
    else
        throw std::runtime_error("attribute \"" + attribute + "\" not found for \"" + type + "\"!");
}

template<typename ValueType>
std::optional<ValueType> Config::getOptional(std::string_view type, std::string_view attribute) const
{
    std::string current(type);
    std::string key(attribute);

    while (true)
    {
        auto groupValue = data.getOptional(current);

        if (!groupValue)
            return std::nullopt;

        auto& group = groupValue->getGroup();

        if (auto value = group.getOptional(key))
        {
            if (auto converted = convert<ValueType>(*value))
                return *converted;
            else
                throw std::runtime_error("attribute \"" + attribute + "\" of class \"" + current +
                                         "\" has wrong type!");
        }

        auto baseType = group.getOptional("BaseType");

        if (!baseType)
            return std::nullopt;

        if (baseType->isString())
        {
            auto& baseTypeString = baseType->getString();

            if (data.getOptional(baseTypeString) == nullptr)
                throw std::runtime_error("BaseType \"" + baseTypeString + "\" doesn't exist!");

            current = baseTypeString;
        }
        else
            throw std::runtime_error("BaseType of \"" + current + "\" has wrong type!");
    }
}
