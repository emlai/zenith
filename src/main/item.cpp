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
:   Item(id, materialId, Sprite(*Game::itemSpriteSheet, getSpriteTextureRegion(Game::itemConfig, id),
                                getMaterialColor(materialId)))
{
}

Item::Item(boost::string_ref id, boost::string_ref materialId, Sprite&& sprite)
:   Entity(id, Game::itemConfig),
    materialId(materialId.to_string()),
    sprite(std::move(sprite))
{
}

std::string Item::getNamePrefix() const
{
    return pascalCaseToSentenceCase(materialId);
}

void Item::render(Vector2 position) const
{
    sprite.render(position);
}

void Item::renderWielded(Vector2 position) const
{
    sprite.render(position, Vector2(0, Tile::size));
}

Corpse::Corpse(boost::string_ref creatureId)
:   Item(creatureId + "Corpse", "",
         Sprite(*Game::creatureSpriteSheet, getSpriteTextureRegion(Game::creatureConfig, creatureId)))
{
    sprite.setFrame(2);
}

void Corpse::renderWielded(Vector2 position) const
{
    render(position);
}
