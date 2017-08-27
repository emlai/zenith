#include "item.h"
#include "game.h"
#include "gui.h"

Item::Item(boost::string_ref id, boost::string_ref materialId)
:   Entity(id, Game::itemConfig),
    materialId(materialId.to_string()),
    sprite(*Game::itemSpriteSheet, getSpriteTextureRegion(Game::itemConfig, id),
           materialId.empty() ? Color16::none : Color16(Game::materialConfig.get<int>(materialId, "Color")))
{
}

void Item::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);
}
