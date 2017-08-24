#pragma once

#include "entity.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>

class Object : public Entity
{
public:
    Object(boost::string_ref id);
    void render(Window& window, Vector2 position) const;
    Sprite& getSprite() { return sprite; }

private:
    Sprite sprite;
};
