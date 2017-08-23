#pragma once

#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>

class Config;
class Texture;

class Object
{
public:
    Object(boost::string_ref id, const Config& config, const Texture& spriteSheet);
    void render(Window& window, Vector2 position) const;

private:
    Sprite sprite;
};
