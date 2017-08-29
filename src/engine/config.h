#pragma once

#include "utility.h"
#include <libconfig.h++>
#include <boost/optional.hpp>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

class Config
{
public:
    Config() {}
    Config(boost::string_ref fileName);
    template<typename ValueType>
    boost::optional<ValueType> getOptional(boost::string_ref key) const;
    template<typename ValueType>
    ValueType get(boost::string_ref type, boost::string_ref attribute) const;
    template<typename ValueType>
    boost::optional<ValueType> getOptional(boost::string_ref type, boost::string_ref attribute) const;

    std::vector<std::string> getToplevelKeys() const
    {
        std::vector<std::string> keys;

        for (auto& setting : config.getRoot())
            if (!setting.exists("isAbstract") || !static_cast<bool>(setting.lookup("isAbstract")))
                keys.push_back(setting.getName());

        return keys;
    }

    void set(boost::string_ref key, double value)
    {
        config.getRoot().add(key.to_string(), libconfig::Setting::TypeFloat) = value;
    }

    void writeToFile(boost::string_ref filePath);

private:
    template<typename OutputType>
    struct ConversionTraits;
    template<typename OutputType>
    static OutputType convert(const libconfig::Setting& inputData)
    {
        return ConversionTraits<OutputType>()(inputData);
    }

    libconfig::Config config;
};

template<typename OutputType>
struct Config::ConversionTraits
{
    OutputType operator()(const libconfig::Setting& inputData)
    {
        return static_cast<OutputType>(inputData);
    }
};

template<>
struct Config::ConversionTraits<std::string>
{
    std::string operator()(const libconfig::Setting& inputData)
    {
        return static_cast<const char*>(inputData);
    }
};

template<typename ElementType>
struct Config::ConversionTraits<std::vector<ElementType>>
{
    std::vector<ElementType> operator()(const libconfig::Setting& inputData)
    {
        std::vector<ElementType> outputData;

        for (auto it = inputData.begin(), end = inputData.end(); it != end; ++it)
            outputData.push_back(convert<ElementType>(*it));

        return outputData;
    }
};

inline Config::Config(boost::string_ref fileName)
{
    try
    {
        config.readFile(fileName.to_string().c_str());
    }
    catch (const libconfig::FileIOException&)
    {
        throw std::runtime_error("Couldn't open \"" + fileName + "\"!");
    }
    catch (const libconfig::ParseException& exception)
    {
        throw std::runtime_error(std::string("Parse error at ") + exception.getFile() + ":" +
                                 std::to_string(exception.getLine()) + " — " +
                                 exception.getError());
    }
}

template<typename ValueType>
boost::optional<ValueType> Config::getOptional(boost::string_ref key) const
{
    ValueType value;

    if (config.lookupValue(key.to_string(), value))
        return value;
    else
        return boost::none;
}

template<typename ValueType>
ValueType Config::get(boost::string_ref type, boost::string_ref attribute) const
{
    if (auto value = getOptional<ValueType>(type, attribute))
        return std::move(*value);
    else
        throw std::runtime_error("attribute \"" + attribute + "\" not found for \"" + type + "\"!");
}

template<typename ValueType>
boost::optional<ValueType> Config::getOptional(boost::string_ref type, boost::string_ref attribute) const
{
    for (auto current = type;;)
    {
        try
        {
            return convert<ValueType>(config.lookup((current + "." + attribute).c_str()));
        }
        catch (const libconfig::SettingNotFoundException&)
        {
            try
            {
                current = static_cast<const char*>(config.lookup(current + ".BaseType"));
            }
            catch (const libconfig::SettingNotFoundException&)
            {
                return boost::none;
            }
        }
        catch (const libconfig::SettingTypeException&)
        {
            throw std::runtime_error("attribute \"" + attribute + "\" of class \"" + type +
                                     "\" has wrong type!");
        }

        if (!config.exists(current.to_string()))
            throw std::runtime_error("BaseType \"" + current + "\" doesn't exist!");
    }
}

inline void Config::writeToFile(boost::string_ref filePath)
{
    config.writeFile(filePath.to_string().c_str());
}
