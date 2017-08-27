#pragma once

#include "entity.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>

class Window;

class Item : public Entity
{
public:
    Item(boost::string_ref id, boost::string_ref materialId);
    void render(Window& window, Vector2 position) const;
    void renderWielded(Window& window, Vector2 position) const;
    const Sprite& getSprite() const { return sprite; }

private:
    std::string materialId;
    Sprite sprite;
};
