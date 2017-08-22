#pragma once

#include <libconfig.h++>
#include <fstream>
#include <functional>
#include <stdexcept>

class Config
{
public:
    Config(const std::string& fileName);
    template<typename ValueType>
    ValueType get(const std::string& type, const std::string& attribute) const;

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

inline Config::Config(const std::string& fileName)
{
    try
    {
        config.readFile(fileName.c_str());
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
ValueType Config::get(const std::string& type, const std::string& attribute) const
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
                throw std::runtime_error("attribute \"" + attribute +
                                         "\" not found for \"" + type + "\"!");
            }
        }
        catch (const libconfig::SettingTypeException&)
        {
            throw std::runtime_error("attribute \"" + attribute + "\" of class \"" + type +
                                     "\" has wrong type!");
        }

        if (!config.exists(current))
            throw std::runtime_error("BaseType \"" + current + "\" doesn't exist!");
    }
}
