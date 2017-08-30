#include "item.h"
#include "game.h"
#include "gui.h"
#include "tile.h"
#include <iostream>

static Color16 getMaterialColor(boost::string_ref materialId)
{
    if (!materialId.empty())
    {
        try
        {
            return Color16(static_cast<uint16_t>(Game::materialConfig.get<int>(materialId, "Color")));
        }
        catch (const std::runtime_error&)
        {
            if (Game::materialConfig.get<std::string>(materialId, "Color") == "Random")
                return Color16(randInt(Color16::max / 2), randInt(Color16::max / 2), randInt(Color16::max / 2));
            else
                throw;
        }
    }
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

bool Item::isUsable() const
{
    for (auto& component : getComponents())
        if (component->isUsable())
            return true;

    return false;
}

bool Item::use(Creature& user, Game& game)
{
    assert(isUsable());
    bool returnValue = false;

    for (auto& component : getComponents())
        if (component->use(user, *this, game))
            returnValue = true;

    return returnValue;
}

EquipmentSlot Item::getEquipmentSlot() const
{
    auto slotString = getConfig().getOptional<std::string>(getId(), "EquipmentSlot").value_or("Hand");

    if (slotString == "Head") return Head;
    if (slotString == "Torso") return Torso;
    if (slotString == "Hand") return Hand;
    if (slotString == "Legs") return Legs;

    std::cerr << "'" << getId() << "' has unknown EquipmentSlot '" << slotString << "'\n";
    assert(false);
    return Hand;
}

std::string Item::getNameAdjective() const
{
    return pascalCaseToSentenceCase(materialId);
}

void Item::render(Vector2 position) const
{
    sprite.render(position);
}

void Item::renderEquipped(Vector2 position) const
{
    sprite.render(position, Vector2(0, Tile::size));
}

std::string getRandomMaterialId(boost::string_ref itemId)
{
    auto materials = Game::itemConfig.get<std::vector<std::string>>(itemId, "PossibleMaterials");
    return materials.empty() ? "" : randomElement(materials);
}

Corpse::Corpse(std::unique_ptr<Creature> creature)
:   Item(creature->getId() + "Corpse", "",
         Sprite(*Game::creatureSpriteSheet, getSpriteTextureRegion(Game::creatureConfig, creature->getId()))),
    creature(std::move(creature))
{
    sprite.setFrame(2);
}

Corpse::Corpse(boost::string_ref creatureId)
:   Item(creatureId + "Corpse", "",
         Sprite(*Game::creatureSpriteSheet, getSpriteTextureRegion(Game::creatureConfig, creatureId)))
{
    sprite.setFrame(2);
}

void Corpse::renderEquipped(Vector2 position) const
{
    render(position);
}
