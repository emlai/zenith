#include "object.h"
#include "game.h"
#include "gui.h"

Object::Object(boost::string_ref id)
:   Entity(id, Game::objectConfig),
    sprite(*Game::objectSpriteSheet, getSpriteTextureRegion(Game::objectConfig, id))
{
}

bool Object::close()
{
    bool returnValue = false;

    for (auto& component : getComponents())
        if (component->close())
            returnValue = true;

    return returnValue;
}

void Object::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);
}
