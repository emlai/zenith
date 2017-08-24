#pragma once

#include "entity.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>

class Window;

class Item : public Entity
{
public:
    Item(boost::string_ref id);
    void render(Window& window, Vector2 position) const;

private:
    Sprite sprite;
};
