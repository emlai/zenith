#pragma once

#include "engine/texture.h"
#include <boost/utility/string_ref.hpp>

class SaveFile;
class Window;

class Liquid
{
public:
    Liquid(boost::string_ref materialId);
    Liquid(const SaveFile& file);
    void save(SaveFile& file) const;
    void exist();
    bool exists() const;
    void render(Window& window, Vector2 position) const;

private:
    static constexpr double fadeRate = 0.001;

    std::string materialId;
    double fadeLevel;
    Texture texture;
};
