#pragma once

#include "utility.h"
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class ConfigReader;

class Config
{
public:
    Config() {}
    Config(boost::string_ref filePath);
    template<typename ValueType>
    boost::optional<ValueType> getOptional(boost::string_ref key) const;
    template<typename ValueType>
    ValueType get(boost::string_ref type, boost::string_ref attribute) const;
    template<typename ValueType>
    boost::optional<ValueType> getOptional(boost::string_ref type, boost::string_ref attribute) const;
    std::vector<std::string> getToplevelKeys() const;
    void set(boost::string_ref key, bool value) { data.insert(key.to_string(), Value(value)); }
    void set(boost::string_ref key, double value) { data.insert(key.to_string(), Value(value)); }
    void writeToFile(boost::string_ref filePath) const;

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

    using Value = boost::make_recursive_variant<bool, long long, double, std::string,
                                                std::vector<boost::recursive_variant_>,
                                                Group_<boost::recursive_variant_>>::type;
    using Group = Group_<Value>;

    template<typename OutputType>
    struct ConversionTraits;
    template<typename OutputType>
    static boost::optional<OutputType> convert(const Value& value)
    {
        return ConversionTraits<OutputType>()(value);
    }

    template<typename T>
    static boost::optional<T> get(const Value& value)
    {
        try
        {
            return boost::get<T>(value);
        }
        catch (const boost::bad_get&)
        {
            return boost::none;
        }
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
    boost::optional<OutputType> operator()(const Value& value)
    {
        return get<OutputType>(value);
    }
};

template<>
struct Config::ConversionTraits<int>
{
    boost::optional<int> operator()(const Value& value)
    {
        if (auto intValue = get<long long>(value))
            return boost::numeric_cast<int>(*intValue);

        return boost::none;
    }
};

template<>
struct Config::ConversionTraits<unsigned>
{
    boost::optional<unsigned> operator()(const Value& value)
    {
        if (auto intValue = get<long long>(value))
            return boost::numeric_cast<unsigned>(*intValue);

        return boost::none;
    }
};

template<>
struct Config::ConversionTraits<unsigned short>
{
    boost::optional<unsigned short> operator()(const Value& value)
    {
        if (auto intValue = get<long long>(value))
            return boost::numeric_cast<unsigned short>(*intValue);

        return boost::none;
    }
};

template<>
struct Config::ConversionTraits<double>
{
    boost::optional<double> operator()(const Value& value)
    {
        if (auto floatValue = get<double>(value))
            return *floatValue;

        if (auto intValue = get<long long>(value))
            return *intValue;

        return boost::none;
    }
};

template<typename ElementType>
struct Config::ConversionTraits<std::vector<ElementType>>
{
    boost::optional<std::vector<ElementType>> operator()(const Value& value)
    {
        std::vector<ElementType> outputData;
        auto vector = get<std::vector<Value>>(value);

        if (!vector)
            return boost::none;

        for (auto it = vector->begin(), end = vector->end(); it != end; ++it)
            outputData.push_back(*convert<ElementType>(*it));

        return outputData;
    }
};

template<typename ValueType>
boost::optional<ValueType> Config::getOptional(boost::string_ref key) const
{
    return convert<ValueType>(data.at(key.to_string()));
}

template<typename ValueType>
ValueType Config::get(boost::string_ref type, boost::string_ref attribute) const
{
    if (auto value = getOptional<ValueType>(type, attribute))
        return ValueType(std::move(*value));
    else
        throw std::runtime_error("attribute \"" + attribute + "\" not found for \"" + type + "\"!");
}

template<typename ValueType>
boost::optional<ValueType> Config::getOptional(boost::string_ref type, boost::string_ref attribute) const
{
    std::string current = type.to_string();
    std::string key = attribute.to_string();

    while (true)
    {
        auto groupValue = data.getOptional(current);

        if (!groupValue)
            return boost::none;

        auto group = get<Group>(*groupValue);

        if (auto value = group->getOptional(key))
        {
            if (auto converted = convert<ValueType>(*value))
                return *converted;
            else
                throw std::runtime_error("attribute \"" + attribute + "\" of class \"" + current +
                                         "\" has wrong type!");
        }

        auto baseType = group->getOptional("BaseType");

        if (!baseType)
            return boost::none;

        if (auto baseTypeString = get<std::string>(*baseType))
        {
            if (data.getOptional(*baseTypeString) == nullptr)
                throw std::runtime_error("BaseType \"" + *baseTypeString + "\" doesn't exist!");

            current = *baseTypeString;
        }
        else
            throw std::runtime_error("BaseType of \"" + current + "\" has wrong type!");
    }
}
