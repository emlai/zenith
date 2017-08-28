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

bool Object::blocksSight() const
{
    for (auto& component : getComponents())
        if (component->blocksSight())
            return true;

    return Game::objectConfig.get<bool>(getId(), "blocksSight");
}

void Object::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);
}
