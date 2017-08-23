#include "object.h"
#include "gui.h"

Object::Object(boost::string_ref id, const Config& config, const Texture& spriteSheet)
:   sprite(spriteSheet, getSpriteTextureRegion(config, id))
{
}

void Object::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);
}
