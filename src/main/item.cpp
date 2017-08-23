#include "item.h"
#include "game.h"
#include "gui.h"

Item::Item(boost::string_ref id)
:   sprite(*Game::itemSpriteSheet, getSpriteTextureRegion(Game::itemConfig, id))
{
}

void Item::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);
}
