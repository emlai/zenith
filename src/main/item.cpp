#include "item.h"
#include "game.h"
#include "gui.h"

static Color16 getMaterialColor(boost::string_ref materialId)
{
    if (!materialId.empty())
        return Color16(static_cast<uint16_t>(Game::materialConfig.get<int>(materialId, "Color")));
    else
        return Color16::none;
}

Item::Item(boost::string_ref id, boost::string_ref materialId)
:   Entity(id, Game::itemConfig),
    materialId(materialId.to_string()),
    sprite(*Game::itemSpriteSheet, getSpriteTextureRegion(Game::itemConfig, id),
           getMaterialColor(materialId))
{
}

std::string Item::getNamePrefix() const
{
    return pascalCaseToSentenceCase(materialId);
}

void Item::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);
}

void Item::renderWielded(Window& window, Vector2 position) const
{
    sprite.render(window, position, Vector2(0, Tile::size));
}
