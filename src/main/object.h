#pragma once

#include "entity.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>
#include <memory>

class SaveFile;

class Object : public Entity
{
public:
    Object(boost::string_ref id);
    Object(const SaveFile& file);
    void save(SaveFile& file) const;
    bool close();
    bool blocksSight() const;
    void render(Window& window, Vector2 position) const;
    Sprite& getSprite() { return sprite; }

private:
    Sprite sprite;
};
