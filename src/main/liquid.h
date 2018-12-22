#pragma once

#include "engine/texture.h"
#include <string_view>

class SaveFile;
class Window;

class Liquid
{
public:
    Liquid(std::string_view materialId);
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
