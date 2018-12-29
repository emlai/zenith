#pragma once

#include "entity.h"
#include "engine/sprite.h"
#include <string_view>

class SaveFile;

class Object : public Entity
{
public:
    Object(std::string_view id);
    Object(const SaveFile& file);
    void save(SaveFile& file) const;
    bool close();
    bool blocksSight() const;
    void render(Window& window, Vector2 position) const;
    Sprite& getSprite() { return sprite; }

private:
    Sprite sprite;
};
