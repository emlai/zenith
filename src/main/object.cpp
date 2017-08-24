#include "object.h"
#include "game.h"
#include "gui.h"

Object::Object(boost::string_ref id)
:   Entity(id, Game::objectConfig),
    sprite(*Game::objectSpriteSheet, getSpriteTextureRegion(Game::objectConfig, id))
{
}

void Object::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);
}
