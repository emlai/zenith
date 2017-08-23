#pragma once

#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>

class Object
{
public:
    Object(boost::string_ref id);
    void render(Window& window, Vector2 position) const;

private:
    Sprite sprite;
};
